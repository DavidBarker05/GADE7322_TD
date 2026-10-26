#pragma once

#include "CoreMinimal.h"

#include "TowerDefencePawns/Projectile.h"

#include "DamageSpellProjectile.generated.h"

class UNiagaraComponent;

UCLASS(Abstract)
class GADE7322_TD_API ADamageSpellProjectile : public AProjectile
{
    GENERATED_BODY()

public:
    ADamageSpellProjectile();

    const UNiagaraComponent* GetSpellEffect() const { return SpellEffect; }
    UNiagaraComponent* GetSpellEffect() { return SpellEffect; }

private:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = true))
    UNiagaraComponent* SpellEffect;
};
