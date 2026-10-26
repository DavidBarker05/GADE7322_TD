#include "TowerDefencePawns/Attackers/Mage/DamageSpellProjectile.h"

#include "NiagaraComponent.h"

ADamageSpellProjectile::ADamageSpellProjectile()
{
    SpellEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Spell Effect"));
    SpellEffect->SetupAttachment(RootComponent);
}
