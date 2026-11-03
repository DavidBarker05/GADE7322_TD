#include "TowerDefencePawns/Defenders/Ballista/BallistaBarrel.h"

#include "TowerDefencePawns/Components/HitFlashComponent.h"
#include "TowerDefencePawns/Components/OutlineComponent.h"

ABallistaBarrel::ABallistaBarrel()
{
    Pivot = CreateDefaultSubobject<USceneComponent>(TEXT("Pivot"));
    SetRootComponent(Pivot);
    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    Mesh->SetupAttachment(RootComponent);
    Muzzle = CreateDefaultSubobject<USceneComponent>(TEXT("Muzzle"));
    Muzzle->SetupAttachment(RootComponent);
    HitFlashComponent = CreateDefaultSubobject<UHitFlashComponent>(TEXT("Hit Flash Component"));
    OutlineComponent = CreateDefaultSubobject<UOutlineComponent>(TEXT("Outline Component"));
}
