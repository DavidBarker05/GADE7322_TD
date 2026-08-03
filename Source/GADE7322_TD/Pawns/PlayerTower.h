#pragma once

#include "CoreMinimal.h"

#include "Pawns/TowerDefensePawn.h"

#include "PlayerTower.generated.h"

UCLASS(Abstract)
class GADE7322_TD_API APlayerTower : public ATowerDefensePawn
{
    GENERATED_BODY()

public:
    APlayerTower();
};
