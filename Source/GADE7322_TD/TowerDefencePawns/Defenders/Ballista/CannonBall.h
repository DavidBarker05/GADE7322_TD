#pragma once

#include "TDCore.h"

#include "TowerDefencePawns/Projectile.h"

#include "CannonBall.generated.h"

class UNiagaraSystem;

UCLASS(Abstract)
class GADE7322_TD_API ACannonBall : public AProjectile
{
    GENERATED_BODY()

public:
    virtual void FireProjectile(USceneComponent* InTarget, const FVector& LaunchVelocity, int32 InDamage,
                                const TArray<FAny>& LaunchArguments) override;

protected:
    virtual void DamageTarget(ATowerDefencePawn* TargetToDamage) override;

private:
    UPROPERTY(EditDefaultsOnly, Category = "Projectile|Ballista|Cannon|Effects")
    UNiagaraSystem* ExplosionSystem;

    UPROPERTY(BlueprintReadOnly, Category = "Projectile|Ballista", meta = (AllowPrivateAccess = true, Units = "%"))
    float GroundTroopDamageReduction;

    UPROPERTY(BlueprintReadOnly, Category = "Projectile|Ballista|Cannon",
              meta = (AllowPrivateAccess = true, Units = "Centimeters"))
    float ExplosionRadius;
};
