#include "TowerDefencePawns/Attackers/ProceduralEnemy/ProceduralParts/GolemPart.h"

#include "TowerDefencePawns/Components/HitFlashComponent.h"

AGolemPart::AGolemPart()
{
    PrimaryActorTick.bCanEverTick = false;
    SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("Root")));
    HitFlashComponent = CreateDefaultSubobject<UHitFlashComponent>(TEXT("Hit Flash Component"));
}

void AGolemPart::BeginPlay()
{
    Super::BeginPlay();
    GetComponents(Rocks);
    for (UStaticMeshComponent* Rock : Rocks)
    {
        Rock->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        Rock->SetCanEverAffectNavigation(false);
        Rock->SetGenerateOverlapEvents(false);
    }
}

void AGolemPart::EnableMesh() { SetRocksActive(true); }

void AGolemPart::DisableMesh() { SetRocksActive(false); }

void AGolemPart::StartCollapse(float LinearImpulse, float SpinDegrees)
{
    Collapse.Begin(Rocks, LinearImpulse, SpinDegrees);
}

void AGolemPart::StopCollapse() { Collapse.End(); }

void AGolemPart::SetRocksActive(bool bActive)
{
    for (UStaticMeshComponent* Rock : Rocks)
    {
        Rock->SetVisibility(bActive);
        Rock->SetComponentTickEnabled(bActive);
    }
}
