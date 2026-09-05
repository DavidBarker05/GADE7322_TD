// ReSharper disable CppParameterMayBeConst
#include "TowerDefencePawns/Defenders/Warrior/AI/WarriorAIController.h"

#include "CustomLog.h"
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
            IsValid(AttackTarget) && AttackTarget->IsPawnActive() && AttackTarget->GetHealthComponent()->IsAlive() &&
            FVector::Dist2D(WPawn->GetActorLocation(), AttackTarget->GetActorLocation()) -
                    AttackTarget->GetOccupiedRadius() <=
                WPawn->GetAttackRadius() + KINDA_SMALL_NUMBER)
        {
            if (const FVector ToTarget = AttackTarget->GetActorLocation() - WPawn->GetActorLocation();
                !ToTarget.IsNearlyZero())
                WPawn->SetActorRotation(FRotator(0.0f, ToTarget.Rotation().Yaw, 0.0f));
            return;
        }
        if (TimeSinceLastVisionUpdate < 1.0f / VisionUpdateFrequency + KINDA_SMALL_NUMBER)
        {
            TimeSinceLastVisionUpdate += DeltaTime;
            return;
        }
        TimeSinceLastVisionUpdate = 0.0f;
        ATowerDefencePawn* Closest = SelectClosestTarget(GetVisiblePawns(), WPawn);
        TD_LOG_INFO(TEXT("AWarriorAIController::Tick -> %s VisiblePawns.Num() = %d, selected target = %s"),
                    *WPawn->GetName(), GetVisiblePawns().Num(), Closest ? *Closest->GetName() : TEXT("nullptr"));
        WPawn->SetAttackTarget(Closest);
    }
}

AWarrior* AWarriorAIController::GetWarrior() const { return GetPawn<AWarrior>(); }

void AWarriorAIController::SetControllerActive(bool bActive)
{
    Super::SetControllerActive(bActive);
    TimeSinceLastVisionUpdate = 0.0f;
}
