#pragma once

#include "TDCore.h"

#include "TowerDefencePawns/Weapon.h"

#include "WarriorWeapon.generated.h"

class UOutlineComponent;

UCLASS(Abstract)
class AWarriorWeapon : public AWeapon
{
    GENERATED_BODY()

public:
    AWarriorWeapon();

    const UOutlineComponent* GetOutlineComponent() const { return OutlineComponent; }
    UOutlineComponent* GetOutlineComponent() { return OutlineComponent; }

private:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = true))
    UOutlineComponent* OutlineComponent;
};
