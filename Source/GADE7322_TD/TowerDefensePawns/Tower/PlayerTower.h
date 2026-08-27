#pragma once

#include "CoreMinimal.h"

#include "TowerDefensePawns/TowerDefensePawn.h"

#include "PlayerTower.generated.h"

class UStaticMeshComponent;

UCLASS(Abstract)
class GADE7322_TD_API APlayerTower : public ATowerDefensePawn
{
    GENERATED_BODY()

public:
    APlayerTower();

private:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    UStaticMeshComponent* TowerMesh;
};
