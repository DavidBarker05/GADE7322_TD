#pragma once

#include "CoreMinimal.h"

#include "Pawns/TowerDefensePawn.h"

#include "EnemyTroop.generated.h"

UCLASS()
class GADE7322_TD_API AEnemyTroop : public ATowerDefensePawn
{
    GENERATED_BODY()

public:
    AEnemyTroop();

public:
    int32 GetCurrencyOnDeath() const;

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Currency",
              meta = (AllowPrivateAccess = "true", ClampMin = "0", ClampMax = "0"))
    int32 CurrencyOnDeath;
};
