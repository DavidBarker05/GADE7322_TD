#include "TowerDefencePawns/Tower/PlayerTower.h"

#include "DamageComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AIPerceptionTypes.h"
#include "TowerDefencePawns/AI/ProximityPerception/AISenseConfig_Proximity.h"

APlayerTower::APlayerTower()
{
    PawnDisplayName = TEXT("Main Tower");
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

void APlayerTower::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
    if (!IsValid(Actor)) return;
    if (ATowerDefencePawn* TDPawn = Cast<ATowerDefencePawn>(Actor))
    {
        if (!Stimulus.WasSuccessfullySensed()) VisiblePawns.Remove(TDPawn);
        else if (TDPawn->IsPawnActive()) VisiblePawns.AddUnique(TDPawn);
    }
}

void APlayerTower::UpdateAttackTargets()
{
    for (int32 i = 0; i < 3; ++i)
    {
        const ATowerDefencePawn* Current = AttackTargets[i];
        if (IsValid(Current) && Current->IsPawnActive() &&
            FVector::Dist2D(GetActorLocation(), Current->GetActorLocation()) - Current->GetOccupiedRadius() <=
                AttackRadius + KINDA_SMALL_NUMBER)
            continue;

        ATowerDefencePawn* NewTarget = nullptr;
        float ClosestDistSqr = TNumericLimits<float>::Max();

        for (int32 j = VisiblePawns.Num() - 1; j >= 0; --j)
        {
            ATowerDefencePawn* Candidate = VisiblePawns[j];
            if (!IsValid(Candidate) || !Candidate->IsPawnActive())
            {
                VisiblePawns.RemoveAtSwap(j, EAllowShrinking::No);
                continue;
            }
            if (AttackTargets.Contains(Candidate)) continue;

            const float DistSqr = FVector::DistSquared2D(GetActorLocation(), Candidate->GetActorLocation());
            if (DistSqr < ClosestDistSqr)
            {
                ClosestDistSqr = DistSqr;
                NewTarget = Candidate;
            }
        }

        if (NewTarget) SetAttackTarget(i, NewTarget);
        else AttackTargets[i] = nullptr;
    }
}
