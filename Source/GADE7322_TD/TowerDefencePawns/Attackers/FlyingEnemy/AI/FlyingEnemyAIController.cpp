#include "TowerDefencePawns/Attackers/FlyingEnemy/AI/FlyingEnemyAIController.h"

#include "Perception/AIPerceptionTypes.h"
#include "TowerDefencePawns/AI/ProximityPerception/AISenseConfig_Proximity.h"
#include "TowerDefencePawns/AI/TargetSelectionFunctions.h"
#include "TowerDefencePawns/Attackers/FlyingEnemy/FlyingEnemy.h"
#include "TowerDefencePawns/Tower/PlayerTower.h"

AFlyingEnemyAIController::AFlyingEnemyAIController()
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

void AFlyingEnemyAIController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    if (AFlyingEnemy* FlyingEnemy = GetFlyingEnemy())
    {
        if (!FlyingEnemy->IsPawnActive()) return;
        if (const ATowerDefencePawn* AttackTarget = FlyingEnemy->GetAttackTarget();
            IsValid(AttackTarget) && AttackTarget->IsPawnActive() && AttackTarget->GetHealthComponent()->IsAlive() &&
            (AttackTarget->IsA<APlayerTower>() ||
             FVector::Dist2D(FlyingEnemy->GetActorLocation(), AttackTarget->GetActorLocation()) -
                     AttackTarget->GetOccupiedRadius() <=
                 FlyingEnemy->GetAttackRadius() + KINDA_SMALL_NUMBER))
        {
            if (const FVector ToTarget = AttackTarget->GetActorLocation() - FlyingEnemy->GetActorLocation();
                !ToTarget.IsNearlyZero())
                FlyingEnemy->SetActorRotation(FRotator(0.0f, ToTarget.Rotation().Yaw, 0.0f));
            return;
        }
        if (TimeSinceLastVisionUpdate < 1.0f / VisionUpdateFrequency + KINDA_SMALL_NUMBER)
        {
            TimeSinceLastVisionUpdate += DeltaTime;
            return;
        }
        TimeSinceLastVisionUpdate = 0.0f;
        ATowerDefencePawn* Closest = SelectClosestTarget(GetVisiblePawns(), FlyingEnemy);
        FlyingEnemy->SetAttackTarget(Closest);
    }
}

AFlyingEnemy* AFlyingEnemyAIController::GetFlyingEnemy() const { return GetPawn<AFlyingEnemy>(); }

void AFlyingEnemyAIController::SetControllerActive(bool bActive)
{
    Super::SetControllerActive(bActive);
    TimeSinceLastVisionUpdate = 0.0f;
}
