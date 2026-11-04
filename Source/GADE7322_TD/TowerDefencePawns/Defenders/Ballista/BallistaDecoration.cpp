#include "TowerDefencePawns/Defenders/Ballista/BallistaDecoration.h"

#include "TowerDefencePawns/Components/HitFlashComponent.h"
#include "TowerDefencePawns/Components/OutlineComponent.h"

ABallistaDecoration::ABallistaDecoration()
{
    SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("Root")));
    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    Mesh->SetupAttachment(RootComponent);
    HitFlashComponent = CreateDefaultSubobject<UHitFlashComponent>(TEXT("Hit Flash Component"));
    OutlineComponent = CreateDefaultSubobject<UOutlineComponent>(TEXT("Outline Component"));
}
