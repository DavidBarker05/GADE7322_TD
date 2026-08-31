#pragma once

#include "CoreMinimal.h"

#include "TowerDefencePawns/TowerDefencePawn.h"

#include "Defender.generated.h"

class ADefenderSpot;
class UBoxComponent;

UCLASS(Abstract)
class GADE7322_TD_API ADefender : public ATowerDefencePawn
{
    GENERATED_BODY()

public:
    ADefender();

    int32 GetCost() const { return Cost; }

    int32 GetSellPrice() const { return SellPrice; }

    ADefender& SetSpawnPoint(ADefenderSpot* Point)
    {
        SpawnPoint = Point;
        return *this;
    }

    const ADefenderSpot* GetSpawnPoint() const { return SpawnPoint; }
    ADefenderSpot* GetSpawnPoint() { return SpawnPoint; }

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cost",
              meta = (AllowPrivateAccess = true, ClampMin = 0, UIMin = 0))
    int32 Cost = 0;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cost",
              meta = (AllowPrivateAccess = true, ClampMin = 0, UIMin = 0))
    int32 SellPrice = 0;

private:
    UPROPERTY(BlueprintReadWrite, Category = "Defender", meta = (AllowPrivateAccess = true))
    ADefenderSpot* SpawnPoint = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = true))
    UBoxComponent* BoxCollider = nullptr;

    UPROPERTY(EditDefaultsOnly, Category = "Collision", meta = (AllowPrivateAccess = true))
    FVector ColliderHalfExtents {50.0f, 50.0f, 50.0f};
};
