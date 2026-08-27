#include "TowerDefensePawns/Attackers/Attacker.h"

AAttacker::AAttacker() { PawnDisplayName = TEXT("EnemyTroop"); }

int32 AAttacker::GetCurrencyOnDeath() const { return CurrencyOnDeath; }
