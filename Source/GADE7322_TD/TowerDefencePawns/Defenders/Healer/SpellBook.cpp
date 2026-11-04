#include "TowerDefencePawns/Defenders/Healer/SpellBook.h"

#include "TowerDefencePawns/Components/HitFlashComponent.h"
#include "TowerDefencePawns/Components/OutlineComponent.h"

ASpellBook::ASpellBook()
{
    SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("Root")));
    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    Mesh->SetupAttachment(RootComponent);
    HitFlashComponent = CreateDefaultSubobject<UHitFlashComponent>(TEXT("Hit Flash Component"));
    OutlineComponent = CreateDefaultSubobject<UOutlineComponent>(TEXT("Outline Component"));
}
