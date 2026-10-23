#pragma once

#include "CoreMinimal.h"

#include "BlueprintFunctionLibraryEnums.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "TowerDefencePawns/Defenders/Defender.h"

#include "DefenderBlueprintFunctionLibrary.generated.h"

UCLASS()
class GADE7322_TD_API UDefenderBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(
        BlueprintCallable, Category = "TowerDefencePawn|Defender",
        meta = (ToolTip =
                    "Gets the name of the Defender for the specified level if the defender class and level are valid",
                ExpandEnumAsExecs = "OutputPins"))
    static FName GetDefenderName(TSubclassOf<ADefender> DefenderClass, int32 Level, EIsValidOutputPins& OutputPins);

    UFUNCTION(
        BlueprintCallable, Category = "TowerDefencePawn|Defender",
        meta = (ToolTip =
                    "Gets the cost of the Defender for the specified level if the defender class and level are valid",
                ExpandEnumAsExecs = "OutputPins"))
    static int32 GetDefenderCost(TSubclassOf<ADefender> DefenderClass, int32 Level, EIsValidOutputPins& OutputPins);
};
