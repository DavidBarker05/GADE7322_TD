#include "TowerDefencePawns/Attackers/Attacker.h"

#include "Events/EventBus.h"
#include "NiagaraComponent.h"
#include "TowerDefencePawns/Components/HealthComponent.h"
#include "TowerDefencePawns/Tower/PlayerTower.h"

AAttacker::AAttacker()
{
    PawnDisplayName = TEXT("EnemyTroop");
    BoostEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Boost Effect"));
    BoostEffect->SetupAttachment(RootComponent);
    BoostEffect->bAutoActivate = false;
}

void AAttacker::Attack(ATowerDefencePawn* Other)
{
    if (!IsValid(Other) || !Other->IsPawnActive() || Other->GetHealthComponent()->IsDead()) return;
    if (!bHasBeenLeaked && Other->IsA<APlayerTower>())
    {
        bHasBeenLeaked = true;
        BROADCAST_EVENT(TEXT("LeakEvent"));
    }
    Super::Attack(Other);
}

void AAttacker::DoOnSetActive(bool bActive)
{
    Super::DoOnSetActive(bActive);
    bHasBeenLeaked = false;
}
