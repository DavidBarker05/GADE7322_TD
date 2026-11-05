#pragma once

#include "TDCore.h"

#include "TowerDefencePawns/Attackers/ProceduralEnemy/ProceduralParts/GolemPart.h"

#include "GolemLeg.generated.h"

UCLASS(Abstract)
class GADE7322_TD_API AGolemLeg : public AGolemPart
{
    GENERATED_BODY()

public:
    float GetSpeed() const { return Speed; }

    float GetMaxStrideAngle() const { return MaxStrideAngle; }

    float GetBodyLowestOffset() const { return BodyLowestOffset; }
    float GetBodyHighestOffset() const { return BodyHighestOffset; }

private:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Golem|Part|Leg|Speed",
              meta = (AllowPrivateAccess = true, ClampMin = 0.0, UIMin = 0.0, Units = "cm/s"))
    float Speed = 0.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Golem|Part|Leg|Walk",
              meta = (AllowPrivateAccess = true, ClampMin = 0.0, UIMin = 0.0, ClampMax = 90.0, UIMax = 90.0,
                      Units = "Degrees"))
    float MaxStrideAngle = 30.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Golem|Part|Leg|Walk",
              meta = (AllowPrivateAccess = true, Units = "Centimeters"))
    float BodyLowestOffset = -8.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Golem|Part|Leg|Walk",
              meta = (AllowPrivateAccess = true, Units = "Centimeters"))
    float BodyHighestOffset = 0.0f;
};
