#include "TowerDefencePawns/Attackers/Attacker.h"

#include "NiagaraComponent.h"

AAttacker::AAttacker()
{
    PawnDisplayName = TEXT("EnemyTroop");
    BoostEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Boost Effect"));
    BoostEffect->SetupAttachment(RootComponent);
    BoostEffect->bAutoActivate = false;
}
