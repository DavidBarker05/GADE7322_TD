#pragma once

#include "CoreMinimal.h"

#include "TowerDefencePawns/TowerDefencePawn.h"

#include "PlayerTower.generated.h"

class UStaticMeshComponent;

UCLASS(Abstract)
class GADE7322_TD_API APlayerTower : public ATowerDefencePawn
{
    GENERATED_BODY()

public:
    APlayerTower();

private:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    UStaticMeshComponent* TowerMesh;
};
