#include "TowerDefencePawns/Defenders/Healer/FireballProjectile.h"

#include "GameFramework/ProjectileMovementComponent.h"
#include "NiagaraComponent.h"

AFireballProjectile::AFireballProjectile()
{
    GetProjectileMovementComponent()->ProjectileGravityScale = 0.0f;
    FireballEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Fireball Effect"));
    FireballEffect->SetupAttachment(RootComponent);
}
