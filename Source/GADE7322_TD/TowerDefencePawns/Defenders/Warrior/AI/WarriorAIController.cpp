#include "TowerDefencePawns/Defenders/Warrior/AI/WarriorAIController.h"

#include "Perception/AIPerceptionTypes.h"
#include "TowerDefencePawns/AI/ProximityPerception/AISenseConfig_Proximity.h"
#include "TowerDefencePawns/AI/TargetSelectionFunctions.h"
#include "TowerDefencePawns/Defenders/Warrior/Warrior.h"

AWarriorAIController::AWarriorAIController()
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

void AWarriorAIController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    AWarrior* WPawn = GetWarrior();
    if (!ATowerDefencePawn::IsPawnActiveAndAlive(WPawn)) return;
    if (const ATowerDefencePawn* AttackTarget = WPawn->GetAttackTarget();
        ATowerDefencePawn::IsPawnActiveAndAlive(AttackTarget) &&
        FVector::Dist2D(WPawn->GetActorLocation(), AttackTarget->GetActorLocation()) -
                AttackTarget->GetOccupiedRadius() <=
            WPawn->GetAttackRadius() + UE_KINDA_SMALL_NUMBER)
    {
        if (const FVector ToTarget = AttackTarget->GetActorLocation() - WPawn->GetActorLocation();
            !ToTarget.IsNearlyZero())
            WPawn->SetActorRotation(FRotator(0.0f, ToTarget.Rotation().Yaw, 0.0f));
        return;
    }
    if (TimeSinceLastVisionUpdate < 1.0f / VisionUpdateFrequency + UE_KINDA_SMALL_NUMBER)
    {
        TimeSinceLastVisionUpdate += DeltaTime;
        return;
    }
    TimeSinceLastVisionUpdate = 0.0f;
    ATowerDefencePawn* Closest = SelectClosestTarget(GetVisiblePawns(), WPawn);
    WPawn->SetAttackTarget(Closest);
}

AWarrior* AWarriorAIController::GetWarrior() const { return GetPawn<AWarrior>(); }

void AWarriorAIController::SetControllerActive(bool bActive)
{
    Super::SetControllerActive(bActive);
    TimeSinceLastVisionUpdate = 0.0f;
}
