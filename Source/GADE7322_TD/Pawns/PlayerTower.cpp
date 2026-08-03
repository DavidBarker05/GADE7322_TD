#include "Pawns/PlayerTower.h"

APlayerTower::APlayerTower()
{
    PawnDisplayName = TEXT("Main Tower");
    TowerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Tower Mesh"));
    TowerMesh->SetupAttachment(RootComponent);
}
