// ReSharper disable CppParameterMayBeConst
#include "TowerDefencePawns/Attackers/Skeleton/AI/SkeletonAIController.h"

#include "Perception/AIPerceptionTypes.h"
#include "TowerDefencePawns/AI/ProximityPerception/AISenseConfig_Proximity.h"
#include "TowerDefencePawns/AI/TargetSelectionFunctions.h"
#include "TowerDefencePawns/Attackers/Skeleton/SkeletonPawn.h"
#include "TowerDefencePawns/Tower/PlayerTower.h"

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
        if (const ATowerDefencePawn* AttackTarget = SKPawn->GetAttackTarget();
            IsValid(AttackTarget) && AttackTarget->IsPawnActive() && AttackTarget->GetHealthComponent()->IsAlive() &&
            (AttackTarget->IsA<APlayerTower>() ||
             FVector::Dist2D(SKPawn->GetActorLocation(), AttackTarget->GetActorLocation()) -
                     AttackTarget->GetOccupiedRadius() <=
                 SKPawn->GetAttackRadius() + KINDA_SMALL_NUMBER))
        {
            if (const FVector ToTarget = AttackTarget->GetActorLocation() - SKPawn->GetActorLocation();
                !ToTarget.IsNearlyZero())
                SKPawn->SetActorRotation(FRotator(0.0f, ToTarget.Rotation().Yaw, 0.0f));
            return;
        }
        if (TimeSinceLastVisionUpdate < 1.0f / VisionUpdateFrequency + KINDA_SMALL_NUMBER)
        {
            TimeSinceLastVisionUpdate += DeltaTime;
            return;
        }
        TimeSinceLastVisionUpdate = 0.0f;
        ATowerDefencePawn* Closest = SelectClosestTarget(GetVisiblePawns(), SKPawn);
        SKPawn->SetAttackTarget(Closest);
    }
}

ASkeletonPawn* ASkeletonAIController::GetSkeletonPawn() const { return GetPawn<ASkeletonPawn>(); }

void ASkeletonAIController::SetControllerActive(bool bActive)
{
    Super::SetControllerActive(bActive);
    TimeSinceLastVisionUpdate = 0.0f;
}
