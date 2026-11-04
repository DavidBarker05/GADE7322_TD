#pragma once

#include "TDCore.h"

#include "Kismet/BlueprintFunctionLibrary.h"
#include "TowerDefencePawns/TowerDefencePawn.h"

#include "TowerDefencePawnBlueprintFunctionLibrary.generated.h"

UCLASS()
class GADE7322_TD_API UTowerDefencePawnBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintPure, Category = "TowerDefencePawn",
              meta = (ToolTip = "Returns true if the tower defence pawn is valid, active and alive"))
    static bool IsPawnActiveAndAlive(const ATowerDefencePawn* TowerDefencePawn);

    UFUNCTION(BlueprintPure, Category = "TowerDefencePawn",
              meta = (ToolTip = "Returns true if the pawn is a ground troop"))
    static bool IsPawnGroundTroop(const ATowerDefencePawn* TowerDefencePawn);
};
