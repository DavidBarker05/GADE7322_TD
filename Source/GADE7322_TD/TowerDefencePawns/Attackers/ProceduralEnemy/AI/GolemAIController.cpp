#include "TowerDefencePawns/Attackers/ProceduralEnemy/AI/GolemAIController.h"

#include "Perception/AIPerceptionTypes.h"
#include "TowerDefencePawns/AI/ProximityPerception/AISenseConfig_Proximity.h"
#include "TowerDefencePawns/AI/TargetSelectionFunctions.h"
#include "TowerDefencePawns/Attackers/ProceduralEnemy/Golem.h"
#include "TowerDefencePawns/Tower/PlayerTower.h"

AGolemController::AGolemController()
{
    PrimaryActorTick.bCanEverTick = true;
    if (const auto ProxConfig = GetProximityConfig())
    {
        ProxConfig->DetectionRadius = 750.0f;
        ProxConfig->DetectionByAffiliation.bDetectEnemies = true;
        ProxConfig->DetectionByAffiliation.bDetectNeutrals = false;
        ProxConfig->DetectionByAffiliation.bDetectFriendlies = false;
    }
}

void AGolemController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    AGolem* Golem = GetGolem();
    if (!ATowerDefencePawn::IsPawnActiveAndAlive(Golem)) return;
    if (const ATowerDefencePawn* AttackTarget = Golem->GetAttackTarget();
        ATowerDefencePawn::IsPawnActiveAndAlive(AttackTarget) &&
        (AttackTarget->IsA<APlayerTower>() ||
         FVector::Dist2D(Golem->GetActorLocation(), AttackTarget->GetActorLocation()) -
                 AttackTarget->GetOccupiedRadius() <=
             Golem->GetAttackRadius() + UE_KINDA_SMALL_NUMBER))
    {
        if (const FVector ToTarget = AttackTarget->GetActorLocation() - Golem->GetActorLocation();
            !ToTarget.IsNearlyZero())
            Golem->SetActorRotation(FRotator(0.0f, ToTarget.Rotation().Yaw, 0.0f));
        return;
    }
    if (TimeSinceLastVisionUpdate < 1.0f / VisionUpdateFrequency + UE_KINDA_SMALL_NUMBER)
    {
        TimeSinceLastVisionUpdate += DeltaTime;
        return;
    }
    TimeSinceLastVisionUpdate = 0.0f;
    ATowerDefencePawn* Closest = SelectClosestTarget(GetVisiblePawns(), Golem);
    Golem->SetAttackTarget(Closest);
}

void AGolemController::SetControllerActive(bool bActive)
{
    Super::SetControllerActive(bActive);
    TimeSinceLastVisionUpdate = 0.0f;
}

AGolem* AGolemController::GetGolem() const { return GetPawn<AGolem>(); }
