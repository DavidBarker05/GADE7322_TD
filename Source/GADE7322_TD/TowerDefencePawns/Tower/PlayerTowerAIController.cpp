// ReSharper disable CppParameterMayBeConst
#include "TowerDefencePawns/Tower/PlayerTowerAIController.h"

#include "Perception/AIPerceptionTypes.h"
#include "PlayerTower.h"
#include "TargetSelectionFunctions.h"
#include "TowerDefencePawns/AI/ProximityPerception/AISenseConfig_Proximity.h"

APlayerTowerAIController::APlayerTowerAIController()
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

void APlayerTowerAIController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    // if (AWarrior* WPawn = GetWarrior())
    //{
    //     if (!WPawn->IsPawnActive()) return;
    //     if (const ATowerDefencePawn* AttackTarget = WPawn->GetAttackTarget();
    //         IsValid(AttackTarget) && FVector::Dist2D(WPawn->GetActorLocation(), AttackTarget->GetActorLocation()) -
    //                                          AttackTarget->GetOccupiedRadius() <=
    //                                      WPawn->GetAttackRadius() + KINDA_SMALL_NUMBER)
    //         return;
    //     if (TimeSinceLastVisionUpdate < 1.0f / VisionUpdateFrequency + KINDA_SMALL_NUMBER)
    //     {
    //         TimeSinceLastVisionUpdate += DeltaTime;
    //         return;
    //     }
    //     TimeSinceLastVisionUpdate = 0.0f;
    //     const ATowerDefencePawn* Closest = SelectClosestTarget(GetVisiblePawns(), WPawn);
    //     WPawn->SetAttackTarget(Closest);
    // }
}

APlayerTower* APlayerTowerAIController::GetPlayerTower() const { return Cast<APlayerTower>(GetPawn()); }

void APlayerTowerAIController::SetControllerActive(bool bActive)
{
    Super::SetControllerActive(bActive);
    TimeSinceLastVisionUpdate = 0.0f;
}
