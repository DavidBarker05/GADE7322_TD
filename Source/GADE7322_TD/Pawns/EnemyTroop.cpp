#include "Pawns/EnemyTroop.h"

AEnemyTroop::AEnemyTroop() { PawnDisplayName = TEXT("EnemyTroop"); }

int32 AEnemyTroop::GetCurrencyOnDeath() const { return CurrencyOnDeath; }
