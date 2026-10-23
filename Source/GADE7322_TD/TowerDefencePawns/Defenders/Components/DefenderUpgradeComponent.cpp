#include "TowerDefencePawns/Defenders/Components/DefenderUpgradeComponent.h"

#include "CustomLog.h"

FDefenderLevel UDefenderUpgradeComponent::GetCurrent() const
{
    if (CurrentLevel == 0) return BaseLevel;
    if (CurrentLevel == 1) return FirstUpgrade;
    return SecondUpgrade;
}

void UDefenderUpgradeComponent::SetLevel(int32 Level)
{
    if (Level < 0 || Level > 2) return;
    CurrentLevel = Level;
    OnLevelChanged.Broadcast(CurrentLevel);
}

void UDefenderUpgradeComponent::Upgrade() { SetLevel(CurrentLevel + 1); }
