#include "TowerDefencePawns/Defenders/Ballista/Ballista.h"

#include "HealthComponent.h"
#include "HitFlashComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AIPerceptionTypes.h"
#include "TowerDefencePawns/AI/ProximityPerception/AISenseConfig_Proximity.h"
#include "TowerDefencePawns/AI/TargetSelectionFunctions.h"

ABallista::ABallista()
{
    PawnDisplayName = TEXT("Ballista");
    OccupiedRadius = 200.0f;
    StandMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Stand Mesh"));
    StandMesh->SetupAttachment(RootComponent);
    PivotPoint = CreateDefaultSubobject<USceneComponent>(TEXT("Pivot Point"));
    PivotPoint->SetupAttachment(RootComponent); // Don't attach to stand so doesn't affect scale if resize stand
    BallistaMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Ballista Mesh"));
    BallistaMesh->SetupAttachment(PivotPoint);
    // ^ So that we can pivot around the point rather than the mesh because mesh isn't centred
    PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception Component"));
    ProximityConfig = CreateDefaultSubobject<UAISenseConfig_Proximity>(TEXT("Proximity Config"));
    ProximityConfig->DetectionRadius = AttackRadius;
    ProximityConfig->DetectionByAffiliation.bDetectEnemies = true;
    ProximityConfig->DetectionByAffiliation.bDetectFriendlies = false;
    ProximityConfig->DetectionByAffiliation.bDetectNeutrals = false;
    PerceptionComponent->ConfigureSense(*ProximityConfig);
    PerceptionComponent->SetDominantSense(ProximityConfig->GetSenseImplementation());
    PerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &ABallista::OnTargetPerceptionUpdated);
    CurrentTeam = EAITeam::RangedDefender;
}

void ABallista::BeginPlay()
{
    Super::BeginPlay();
    SetPawnActive(true);
    HitFlashComponent->BindMaterials();
    if (PivotPoint) DefaultPivotRotation = PivotPoint->GetRelativeRotation();
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
    UpdatePivotRotation(DeltaTime);
    StartAttack();
}

void ABallista::StartAttack()
{
    if (!IsValid(CurrentAttackTarget) || !bCanAttack) return;
    bCanAttack = false;
    Attack(CurrentAttackTarget);
    // TODO: fire the actual projectile/shot here
    GetWorldTimerManager().SetTimer(AttackTimerHandle, [this]() -> void { bCanAttack = true; }, AttackCooldown, false);
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
    StandMesh->SetVisibility(bActive);
    StandMesh->SetComponentTickEnabled(bActive);
    StandMesh->SetCollisionEnabled(bActive ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision);
    StandMesh->SetCollisionResponseToAllChannels(bActive ? ECR_Block : ECR_Ignore);
    BallistaMesh->SetVisibility(bActive);
    BallistaMesh->SetComponentTickEnabled(bActive);
    BallistaMesh->SetCollisionEnabled(bActive ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision);
    BallistaMesh->SetCollisionResponseToAllChannels(bActive ? ECR_Block : ECR_Ignore);
    if (bActive)
    {
        StandMesh->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Ignore);
        StandMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
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

void ABallista::UpdatePivotRotation(float DeltaTime)
{
    if (!PivotPoint) return;
    FRotator TargetRelativeRotation = DefaultPivotRotation;
    if (IsValid(CurrentAttackTarget))
    {
        FVector Direction = CurrentAttackTarget->GetActorLocation() - PivotPoint->GetComponentLocation();
        Direction.Z = 0.0f;
        if (!Direction.IsNearlyZero())
        {
            const USceneComponent* Parent = PivotPoint->GetAttachParent();
            const FVector LocalDirection =
                Parent ? Parent->GetComponentTransform().InverseTransformVectorNoScale(Direction) : Direction;
            TargetRelativeRotation = FRotator(0.0f, LocalDirection.Rotation().Yaw, 0.0f);
        }
    }
    const FRotator NewRotation =
        FMath::RInterpConstantTo(PivotPoint->GetRelativeRotation(), TargetRelativeRotation, DeltaTime, RotationSpeed);
    PivotPoint->SetRelativeRotation(NewRotation);
}
