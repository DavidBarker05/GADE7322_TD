#include "TowerDefencePawns/Tower/PlayerTower.h"

#include "Components/BoxComponent.h"
#include "CustomLog.h"
#include "HealthComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AIPerceptionTypes.h"
#include "TDCollisionChannels.h"
#include "TowerDefencePawns/AI/ProximityPerception/AISenseConfig_Proximity.h"
#include "TowerDefencePawns/AI/TargetSelectionFunctions.h"

APlayerTower::APlayerTower()
{
    PawnDisplayName = TEXT("Main Tower");
    OccupiedRadius = 200.0f;
    TowerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Tower Mesh"));
    TowerMesh->SetupAttachment(RootComponent);
    AttackTargets.Init(nullptr, 3);
    TimerHandles.SetNum(3);
    PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception Component"));
    ProximityConfig = CreateDefaultSubobject<UAISenseConfig_Proximity>(TEXT("Proximity Config"));
    ProximityConfig->DetectionRadius = AttackRadius;
    ProximityConfig->DetectionByAffiliation.bDetectNeutrals = true;
    PerceptionComponent->ConfigureSense(*ProximityConfig);
    PerceptionComponent->SetDominantSense(ProximityConfig->GetSenseImplementation());
    PerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &APlayerTower::OnTargetPerceptionUpdated);
    BoxCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Collider"));
    BoxCollider->SetupAttachment(RootComponent);
    BoxCollider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    BoxCollider->SetCollisionObjectType(ECC_WorldDynamic);
    BoxCollider->SetCollisionResponseToAllChannels(ECR_Ignore);
    BoxCollider->SetCollisionResponseToChannel(MouseClickTraceChannel, ECR_Block);
    CurrentTeam = EAITeam::Defender;
}

void APlayerTower::BeginPlay()
{
    Super::BeginPlay();
    SetPawnActive(true);
    if (OccupiedRadius <= 0.0f && TowerMesh)
    {
        const FVector Extent = TowerMesh->Bounds.BoxExtent;
        OccupiedRadius = FMath::Min(Extent.X, Extent.Y);
    }
}

void APlayerTower::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    TimeSinceLastTargetUpdate += DeltaTime;
    if (TimeSinceLastTargetUpdate >= 1.0f / TargetUpdateFrequency)
    {
        TimeSinceLastTargetUpdate = 0.0f;
        UpdateAttackTargets();
    }
    StartAttack();
}

void APlayerTower::StartAttack()
{
    for (int32 i = 0; i < 3; ++i)
    {
        if (IsValid(AttackTargets[i]) && CanAttackTarget[i])
        {
            CanAttackTarget[i] = false;
            Attack(AttackTargets[i]);
            // TODO: Fire laser
            GetWorldTimerManager().SetTimer(
                TimerHandles[i], [this, i]() -> void { CanAttackTarget[i] = true; }, AttackCooldown, false);
        }
    }
}

void APlayerTower::DoUpdatePerceptionOnTeamChange()
{
    if (UAIPerceptionSystem* PerceptionSys = UAIPerceptionSystem::GetCurrent(GetWorld()))
        PerceptionSys->UpdateListener(*PerceptionComponent);
    PerceptionComponent->ForgetAll();
    VisiblePawns.Empty();
    AttackTargets.Empty();
}

void APlayerTower::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
    if (!IsValid(Actor)) return;
    if (ATowerDefencePawn* TDPawn = Cast<ATowerDefencePawn>(Actor))
    {
        if (!Stimulus.WasSuccessfullySensed()) VisiblePawns.Remove(TDPawn);
        else if (TDPawn->IsPawnActive() && TDPawn->GetHealthComponent()->IsAlive()) VisiblePawns.AddUnique(TDPawn);
    }
}

void APlayerTower::UpdateAttackTargets()
{
    for (int32 i = 0; i < 3; ++i)
    {
        if (const ATowerDefencePawn* Current = AttackTargets[i];
            IsValid(Current) && Current->IsPawnActive() && Current->GetHealthComponent()->IsAlive() &&
            FVector::Dist2D(GetActorLocation(), Current->GetActorLocation()) - Current->GetOccupiedRadius() <=
                AttackRadius + KINDA_SMALL_NUMBER)
            continue;
        if (ATowerDefencePawn* NewTarget = SelectClosestTarget(VisiblePawns, this)) SetAttackTarget(i, NewTarget);
        else AttackTargets[i] = nullptr;
    }
}
