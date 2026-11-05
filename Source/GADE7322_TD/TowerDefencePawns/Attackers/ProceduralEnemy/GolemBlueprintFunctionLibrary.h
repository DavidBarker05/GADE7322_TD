#pragma once

#include "TDCore.h"

#include "Kismet/BlueprintFunctionLibrary.h"
#include "TowerDefencePawns/Attackers/ProceduralEnemy/Golem.h"

#include "GolemBlueprintFunctionLibrary.generated.h"

UCLASS()
class GADE7322_TD_API UGolemBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintPure, Category = "TowerDefencePawn|Attacker|Golem")
    static EGolemPartStrength GetRandomPartStrength();
};
