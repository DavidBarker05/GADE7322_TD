#pragma once

#include "TDCore.h"

#include "TowerDefencePawns/Attackers/ProceduralEnemy/ProceduralParts/GolemPart.h"

#include "GolemArm.generated.h"

UCLASS(Abstract)
class GADE7322_TD_API AGolemArm : public AGolemPart
{
    GENERATED_BODY()

public:
    int32 GetDamage() const { return Damage; }

    float GetIdleLowestOffset() const { return IdleLowestOffset; }
    float GetIdleHighestOffset() const { return IdleHighestOffset; }

private:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Golem|Part|Arm|Damage",
              meta = (AllowPrivateAccess = true, ClampMin = 0, UIMin = 0))
    int32 Damage = 0;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Golem|Part|Arm|Idle",
              meta = (AllowPrivateAccess = true, Units = "Centimeters"))
    float IdleLowestOffset = -4.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Golem|Part|Arm|Idle",
              meta = (AllowPrivateAccess = true, Units = "Centimeters"))
    float IdleHighestOffset = 4.0f;
};
