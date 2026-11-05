#include "TowerDefencePawns/Attackers/ProceduralEnemy/ProceduralParts/GolemArm.h"

#include "TowerDefencePawns/Components/HitFlashComponent.h"

AGolemArm::AGolemArm()
{
    SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("Root")));
    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    Mesh->SetupAttachment(RootComponent);
    HitFlashComponent = CreateDefaultSubobject<UHitFlashComponent>(TEXT("Hit Flash Component"));
}

void AGolemArm::EnableMesh()
{
    Mesh->SetVisibility(true);
    Mesh->SetComponentTickEnabled(true);
}

void AGolemArm::DisableMesh()
{
    Mesh->SetVisibility(false);
    Mesh->SetComponentTickEnabled(false);
}
