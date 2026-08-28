#pragma once

#include "CoreMinimal.h"

#include "TowerDefencePawns/TowerDefencePawn.h"

#include "Defender.generated.h"

UCLASS(Abstract)
class GADE7322_TD_API ADefender : public ATowerDefencePawn
{
    GENERATED_BODY()

public:
    ADefender();

public:
    int32 GetCost() const { return Cost; }

    int32 GetSellPrice() const { return SellPrice; }

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cost",
              meta = (AllowPrivateAccess = "true", ClampMin = "0", UIMin = "0"))
    int32 Cost = 0;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cost",
              meta = (AllowPrivateAccess = "true", ClampMin = "0", UIMin = "0"))
    int32 SellPrice = 0;
};
