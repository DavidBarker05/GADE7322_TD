#include "TowerDefencePawns/Attackers/ProceduralEnemy/ProceduralParts/GolemLeg.h"

#include "TowerDefencePawns/Components/HitFlashComponent.h"

AGolemLeg::AGolemLeg()
{
    SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("Root")));
    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    Mesh->SetupAttachment(RootComponent);
    HitFlashComponent = CreateDefaultSubobject<UHitFlashComponent>(TEXT("Hit Flash Component"));
}

void AGolemLeg::EnableMesh()
{
    Mesh->SetVisibility(true);
    Mesh->SetComponentTickEnabled(true);
}

void AGolemLeg::DisableMesh()
{
    Mesh->SetVisibility(false);
    Mesh->SetComponentTickEnabled(false);
}
