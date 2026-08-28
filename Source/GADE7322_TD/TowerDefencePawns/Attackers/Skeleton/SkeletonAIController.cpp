#include "TowerDefencePawns/Attackers/Skeleton/SkeletonAIController.h"

#include "Perception/AIPerceptionTypes.h"
#include "PlayerTower.h"
#include "TowerDefencePawns/AI/ProximityPerception/AISenseConfig_Proximity.h"
#include "TowerDefencePawns/AI/TargetSelectionFunctions.h"
#include "TowerDefencePawns/Attackers/Skeleton/SkeletonPawn.h"

ASkeletonAIController::ASkeletonAIController()
{
    PrimaryActorTick.bCanEverTick = true;
    if (const auto ProxConfig = GetProximityConfig())
    {
        ProxConfig->DetectionRadius = 500.0f;
        ProxConfig->DetectionByAffiliation.bDetectEnemies = true;
        ProxConfig->DetectionByAffiliation.bDetectNeutrals = false;
        ProxConfig->DetectionByAffiliation.bDetectFriendlies = false;
    }
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
        const ATowerDefencePawn* Closest = SelectClosestTarget(GetVisiblePawns(), SKPawn);
        SKPawn->SetAttackTarget(Closest);
    }
}

ASkeletonPawn* ASkeletonAIController::GetSkeletonPawn() const { return Cast<ASkeletonPawn>(GetPawn()); }

void ASkeletonAIController::SetControllerActive(bool bActive)
{
    Super::SetControllerActive(bActive);
    TimeSinceLastVisionUpdate = 0.0f;
}
