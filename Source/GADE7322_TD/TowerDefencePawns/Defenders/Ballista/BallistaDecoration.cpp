#include "TowerDefencePawns/Defenders/Ballista/BallistaDecoration.h"

#include "TowerDefencePawns/Components/HitFlashComponent.h"
#include "TowerDefencePawns/Components/OutlineComponent.h"

ABallistaDecoration::ABallistaDecoration()
{
    SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("Root")));
    HitFlashComponent = CreateDefaultSubobject<UHitFlashComponent>(TEXT("Hit Flash Component"));
    OutlineComponent = CreateDefaultSubobject<UOutlineComponent>(TEXT("Outline Component"));
}

void ABallistaDecoration::SetVisible(bool bVisible)
{
    if (MeshComponents.IsEmpty()) PopulateMeshes();
    for (auto Mesh : MeshComponents)
    {
        Mesh->SetVisibility(bVisible);
        Mesh->SetComponentTickEnabled(bVisible);
        Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        Mesh->SetCollisionResponseToAllChannels(ECR_Ignore);
        if (bVisible) Mesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    }
}

void ABallistaDecoration::PopulateMeshes() { GetComponents(MeshComponents); }
