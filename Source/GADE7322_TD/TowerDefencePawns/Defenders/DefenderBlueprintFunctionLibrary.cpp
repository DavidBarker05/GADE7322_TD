#include "TowerDefencePawns/Defenders/DefenderBlueprintFunctionLibrary.h"

#include "TowerDefencePawns/Defenders/Components/DefenderUpgradeComponent.h"

FName UDefenderBlueprintFunctionLibrary::GetDefenderName(TSubclassOf<ADefender> DefenderClass, int32 Level,
                                                         EIsValidOutputPins& OutputPins)
{
    if (!DefenderClass || Level < 0 || Level > 2)
    {
        OutputPins = EIsValidOutputPins::NotValid;
        return NAME_None;
    }
    const ADefender* DefaultDefender = DefenderClass->GetDefaultObject<ADefender>();
    if (!DefaultDefender)
    {
        OutputPins = EIsValidOutputPins::NotValid;
        return NAME_None;
    }
    OutputPins = EIsValidOutputPins::Valid;
    if (Level == 0) return DefaultDefender->GetUpgradeComponent()->GetBaseLevel().GetName();
    if (Level == 1) return DefaultDefender->GetUpgradeComponent()->GetFirstUpgrade().GetName();
    return DefaultDefender->GetUpgradeComponent()->GetSecondUpgrade().GetName();
}

int32 UDefenderBlueprintFunctionLibrary::GetDefenderCost(TSubclassOf<ADefender> DefenderClass, int32 Level,
                                                         EIsValidOutputPins& OutputPins)
{
    if (!DefenderClass || Level < 0 || Level > 2)
    {
        OutputPins = EIsValidOutputPins::NotValid;
        return {};
    }
    const ADefender* DefaultDefender = DefenderClass->GetDefaultObject<ADefender>();
    if (!DefaultDefender)
    {
        OutputPins = EIsValidOutputPins::NotValid;
        return {};
    }
    OutputPins = EIsValidOutputPins::Valid;
    if (Level == 0) return DefaultDefender->GetUpgradeComponent()->GetBaseLevel().GetCost();
    if (Level == 1) return DefaultDefender->GetUpgradeComponent()->GetFirstUpgrade().GetCost();
    return DefaultDefender->GetUpgradeComponent()->GetSecondUpgrade().GetCost();
}
