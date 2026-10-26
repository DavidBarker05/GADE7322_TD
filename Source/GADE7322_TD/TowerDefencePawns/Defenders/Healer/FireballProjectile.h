#pragma once

#include "CoreMinimal.h"

#include "TowerDefencePawns/Projectile.h"

#include "FireballProjectile.generated.h"

class UNiagaraComponent;

UCLASS(Abstract)
class GADE7322_TD_API AFireballProjectile : public AProjectile
{
    GENERATED_BODY()

public:
    AFireballProjectile();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = true))
    UNiagaraComponent* FireballEffect;
};
