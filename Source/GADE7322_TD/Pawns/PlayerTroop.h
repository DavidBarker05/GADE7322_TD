#pragma once

#include "CoreMinimal.h"

#include "Pawns/TowerDefensePawn.h"

#include "PlayerTroop.generated.h"

UCLASS()
class GADE7322_TD_API APlayerTroop : public ATowerDefensePawn
{
    GENERATED_BODY()

public:
    APlayerTroop();

public:
    int32 GetCost() const;

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cost",
              meta = (AllowPrivateAccess = "true", ClampMin = "0", UIMin = "0"))
    int32 Cost = 0;
};
