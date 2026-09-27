#include "TowerDefencePawns/Defenders/Ballista/Ballista.h"

#include "Components/BoxComponent.h"
#include "HealthComponent.h"
#include "HitFlashComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AIPerceptionTypes.h"
#include "TDCollisionChannels.h"
#include "TowerDefencePawns/AI/ProximityPerception/AISenseConfig_Proximity.h"
#include "TowerDefencePawns/AI/TargetSelectionFunctions.h"

ABallista::ABallista()
{
    PawnDisplayName = TEXT("Ballista");
    OccupiedRadius = 200.0f;
    BallistaMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Ballista Mesh"));
    BallistaMesh->SetupAttachment(RootComponent);
    PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception Component"));
    ProximityConfig = CreateDefaultSubobject<UAISenseConfig_Proximity>(TEXT("Proximity Config"));
    ProximityConfig->DetectionRadius = AttackRadius;
    ProximityConfig->DetectionByAffiliation.bDetectEnemies = true;
    ProximityConfig->DetectionByAffiliation.bDetectFriendlies = false;
    ProximityConfig->DetectionByAffiliation.bDetectNeutrals = false;
    PerceptionComponent->ConfigureSense(*ProximityConfig);
    PerceptionComponent->SetDominantSense(ProximityConfig->GetSenseImplementation());
    PerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &ABallista::OnTargetPerceptionUpdated);
    BoxCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Collider"));
    BoxCollider->SetupAttachment(RootComponent);
    BoxCollider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    BoxCollider->SetCollisionObjectType(ECC_WorldDynamic);
    BoxCollider->SetCollisionResponseToAllChannels(ECR_Ignore);
    BoxCollider->SetCollisionResponseToChannel(MouseClickTraceChannel, ECR_Block);
    CurrentTeam = EAITeam::RangedDefender;
}

void ABallista::BeginPlay()
{
    Super::BeginPlay();
    SetPawnActive(true);
    HitFlashComponent->BindMaterials();
    if (OccupiedRadius <= 0.0f && BallistaMesh)
    {
        const FVector Extent = BallistaMesh->Bounds.BoxExtent;
        OccupiedRadius = FMath::Min(Extent.X, Extent.Y);
    }
}

void ABallista::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    TimeSinceLastTargetUpdate += DeltaTime;
    if (TimeSinceLastTargetUpdate >= 1.0f / TargetUpdateFrequency)
    {
        TimeSinceLastTargetUpdate = 0.0f;
        UpdateAttackTarget();
    }
    StartAttack();
}

void ABallista::StartAttack()
{
    if (!IsValid(CurrentAttackTarget) || !bCanAttack) return;
    bCanAttack = false;
    Attack(CurrentAttackTarget);
    // TODO: fire the actual projectile/shot here
    GetWorldTimerManager().SetTimer(
        AttackTimerHandle, [this]() -> void { bCanAttack = true; }, AttackCooldown, false);
}

void ABallista::DoOnSetActive(bool bActive)
{
    if (bActive)
    {
        CurrentAttackTarget = nullptr;
        GetWorldTimerManager().ClearTimer(AttackTimerHandle);
        HitFlashComponent->BindMaterials();
    }
    else HitFlashComponent->UnbindMaterials();
    BallistaMesh->SetVisibility(bActive);
    BallistaMesh->SetComponentTickEnabled(bActive);
    BallistaMesh->SetCollisionEnabled(bActive ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision);
    BallistaMesh->SetCollisionResponseToAllChannels(bActive ? ECR_Block : ECR_Ignore);
    if (bActive)
    {
        BallistaMesh->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Ignore);
        BallistaMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    }
    bCanAttack = bActive;
}

void ABallista::DoUpdatePerceptionOnTeamChange()
{
    if (UAIPerceptionSystem* PerceptionSys = UAIPerceptionSystem::GetCurrent(GetWorld()))
        PerceptionSys->UpdateListener(*PerceptionComponent);
    PerceptionComponent->ForgetAll();
    VisiblePawns.Empty();
    CurrentAttackTarget = nullptr;
}

void ABallista::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
    if (!IsValid(Actor)) return;
    if (ATowerDefencePawn* TDPawn = Cast<ATowerDefencePawn>(Actor))
    {
        if (!Stimulus.WasSuccessfullySensed()) VisiblePawns.Remove(TDPawn);
        else if (TDPawn->IsPawnActive() && TDPawn->GetHealthComponent()->IsAlive()) VisiblePawns.AddUnique(TDPawn);
    }
}

void ABallista::UpdateAttackTarget()
{
    if (const ATowerDefencePawn* Current = CurrentAttackTarget;
        IsValid(Current) && Current->IsPawnActive() && Current->GetHealthComponent()->IsAlive() &&
        FVector::Dist2D(GetActorLocation(), Current->GetActorLocation()) - Current->GetOccupiedRadius() <=
            AttackRadius + KINDA_SMALL_NUMBER)
        return;
    if (ATowerDefencePawn* NewTarget = SelectClosestTarget(VisiblePawns, this)) SetAttackTarget(NewTarget);
    else CurrentAttackTarget = nullptr;
}
