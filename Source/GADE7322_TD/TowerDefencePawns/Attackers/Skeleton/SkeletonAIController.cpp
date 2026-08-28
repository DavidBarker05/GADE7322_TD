#include "TowerDefencePawns/Attackers/Skeleton/SkeletonAIController.h"

#include "Components/StateTreeAIComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AIPerceptionTypes.h"
#include "PlayerTower.h"
#include "TowerDefencePawns/Attackers/Skeleton/SkeletonPawn.h"
#include "TowerDefencePawns/ProximityPerception/AISenseConfig_Proximity.h"

ASkeletonAIController::ASkeletonAIController()
{
    PrimaryActorTick.bCanEverTick = true;
    PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception Component"));
    ProximityConfig = CreateDefaultSubobject<UAISenseConfig_Proximity>(TEXT("Proximity Config"));
    ProximityConfig->DetectionRadius = 500.0f;
    ProximityConfig->DetectionByAffiliation.bDetectEnemies = true;
    ProximityConfig->DetectionByAffiliation.bDetectNeutrals = false;
    ProximityConfig->DetectionByAffiliation.bDetectFriendlies = false;
    PerceptionComponent->ConfigureSense(*ProximityConfig);
    PerceptionComponent->SetDominantSense(ProximityConfig->GetSenseImplementation());
    StateTree = CreateDefaultSubobject<UStateTreeAIComponent>(TEXT("State Tree"));
    PerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &ASkeletonAIController::OnTargetPerceptionUpdated);
    PerceptionComponent->SetActive(false);
    StateTree->SetActive(false);
}

void ASkeletonAIController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    if (ASkeletonPawn* SKPawn = GetSkeletonPawn())
    {
        if (!SKPawn->IsPawnActive()) return;
        const ATowerDefencePawn* AttackTarget = SKPawn->GetAttackTarget();
        if (IsValid(AttackTarget) && (AttackTarget->IsA<APlayerTower>() ||
                                      FVector::Dist2D(SKPawn->GetActorLocation(), AttackTarget->GetActorLocation()) -
                                              AttackTarget->GetOccupiedRadius() <=
                                          SKPawn->GetAttackRadius() + KINDA_SMALL_NUMBER))
            return;
        if (TimeSinceLastVisionUpdate < 1.0f / VisionUpdateFrequency + KINDA_SMALL_NUMBER)
        {
            TimeSinceLastVisionUpdate += DeltaTime;
            return;
        }
        TimeSinceLastVisionUpdate = 0.0f;
        const ATowerDefencePawn* Closest = nullptr;
        float ClosestDist = TNumericLimits<float>::Max();
        const FVector PawnLoc = SKPawn->GetActorLocation();
        for (int32 i = VisibleEnemies.Num() - 1; i >= 0; --i)
        {
            const ATowerDefencePawn* Enemy = VisibleEnemies[i];
            if (!IsValid(Enemy) || !Enemy->IsPawnActive())
            {
                VisibleEnemies.RemoveAt(i, EAllowShrinking::No);
                continue;
            }
            const float CurrentDist = FVector::Dist2D(PawnLoc, Enemy->GetActorLocation()) - Enemy->GetOccupiedRadius();
            if (CurrentDist < ClosestDist)
            {
                Closest = Enemy;
                ClosestDist = CurrentDist;
            }
        }
        SKPawn->SetAttackTarget(Closest);
    }
}

ASkeletonPawn* ASkeletonAIController::GetSkeletonPawn() const { return Cast<ASkeletonPawn>(GetPawn()); }

void ASkeletonAIController::SetControllerActive(bool bActive)
{
    if (!bActive)
    {
        PerceptionComponent->ForgetAll();
        VisibleEnemies.Empty();
    }
    PerceptionComponent->SetActive(bActive);
    StateTree->SetActive(bActive);
    TimeSinceLastVisionUpdate = 0.0f;
}

void ASkeletonAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
    if (!IsValid(Actor)) return;
    if (ATowerDefencePawn* TDPawn = Cast<ATowerDefencePawn>(Actor))
    {
        if (Stimulus.WasSuccessfullySensed())
        {
            if (TDPawn->IsPawnActive()) VisibleEnemies.AddUnique(TDPawn);
        }
        else VisibleEnemies.Remove(TDPawn);
    }
}
