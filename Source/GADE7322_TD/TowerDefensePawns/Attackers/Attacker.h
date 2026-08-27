#pragma once

#include "CoreMinimal.h"

#include "TowerDefensePawns/TowerDefensePawn.h"

#include "Attacker.generated.h"

UCLASS(Abstract)
class GADE7322_TD_API AAttacker : public ATowerDefensePawn
{
    GENERATED_BODY()

public:
    AAttacker();

public:
    int32 GetCurrencyOnDeath() const;

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Currency",
              meta = (AllowPrivateAccess = "true", ClampMin = "0", ClampMax = "0"))
    int32 CurrencyOnDeath;
};
