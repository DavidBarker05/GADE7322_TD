// ReSharper disable CppParameterMayBeConst
#include "TowerDefencePawns/Defenders/Warrior/WarriorAIController.h"

#include "Perception/AIPerceptionTypes.h"
#include "TargetSelectionFunctions.h"
#include "TowerDefencePawns/AI/ProximityPerception/AISenseConfig_Proximity.h"
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
    if (AWarrior* WPawn = GetWarrior())
    {
        if (!WPawn->IsPawnActive()) return;
        if (const ATowerDefencePawn* AttackTarget = WPawn->GetAttackTarget();
            IsValid(AttackTarget) && FVector::Dist2D(WPawn->GetActorLocation(), AttackTarget->GetActorLocation()) -
                                             AttackTarget->GetOccupiedRadius() <=
                                         WPawn->GetAttackRadius() + KINDA_SMALL_NUMBER)
            return;
        if (TimeSinceLastVisionUpdate < 1.0f / VisionUpdateFrequency + KINDA_SMALL_NUMBER)
        {
            TimeSinceLastVisionUpdate += DeltaTime;
            return;
        }
        TimeSinceLastVisionUpdate = 0.0f;
        ATowerDefencePawn* Closest = SelectClosestTarget(GetVisiblePawns(), WPawn);
        WPawn->SetAttackTarget(Closest);
    }
}

AWarrior* AWarriorAIController::GetWarrior() const { return Cast<AWarrior>(GetPawn()); }

void AWarriorAIController::SetControllerActive(bool bActive)
{
    Super::SetControllerActive(bActive);
    TimeSinceLastVisionUpdate = 0.0f;
}
