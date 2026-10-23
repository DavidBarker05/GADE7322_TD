#include "TowerDefencePawns/Defenders/AI/DefenderAIController.h"

#include "TowerDefencePawns/AI/ProximityPerception/AISenseConfig_Proximity.h"
#include "TowerDefencePawns/Defenders/Components/DefenderUpgradeComponent.h"
#include "TowerDefencePawns/Defenders/Defender.h"

ADefender* ADefenderAIController::GetDefender() const { return GetPawn<ADefender>(); }

FDefenderAIControllerLevel ADefenderAIController::GetControllerCurrentLevel() const
{
    const ADefender* Defender = GetDefender();
    if (!IsValid(Defender)) return ControllerBaseLevel;
    const int32 CurrentLevel = Defender->GetUpgradeComponent()->GetCurrentLevel();
    if (CurrentLevel == 0) return ControllerBaseLevel;
    if (CurrentLevel == 1) return ControllerFirstUpgrade;
    return ControllerSecondUpgrade;
}

void ADefenderAIController::UpdateControllerLevel()
{
    const FDefenderAIControllerLevel Current = GetControllerCurrentLevel();
    if (const auto ProxConfig = GetProximityConfig())
    {
        ProxConfig->DetectionRadius = Current.GetDetectionRadius();
        ProxConfig->DetectionByAffiliation.bDetectEnemies = Current.DetectEnemies();
        ProxConfig->DetectionByAffiliation.bDetectNeutrals = Current.DetectNeutrals();
        ProxConfig->DetectionByAffiliation.bDetectFriendlies = Current.DetectFriendlies();
    }
}
