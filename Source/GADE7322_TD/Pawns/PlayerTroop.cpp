#include "Pawns/PlayerTroop.h"

APlayerTroop::APlayerTroop()
{
    PawnDisplayName = TEXT("PlayerTroop");
}

int32 APlayerTroop::GetCost() { return Cost; }
