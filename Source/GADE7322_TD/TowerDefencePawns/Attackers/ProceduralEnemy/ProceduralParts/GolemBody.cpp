#include "TowerDefencePawns/Attackers/ProceduralEnemy/ProceduralParts/GolemBody.h"

#include "TowerDefencePawns/Components/HitFlashComponent.h"

AGolemBody::AGolemBody()
{
    SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("Root")));
    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    Mesh->SetupAttachment(RootComponent);
    HitFlashComponent = CreateDefaultSubobject<UHitFlashComponent>(TEXT("Hit Flash Component"));
    HeadAttachmentPoint = CreateDefaultSubobject<USceneComponent>(TEXT("Head Attachment Point"));
    HeadAttachmentPoint->SetupAttachment(RootComponent);
    RightArmAttachmentPoint = CreateDefaultSubobject<USceneComponent>(TEXT("Right Arm Attachment Point"));
    RightArmAttachmentPoint->SetupAttachment(RootComponent);
    LeftArmAttachmentPoint = CreateDefaultSubobject<USceneComponent>(TEXT("Left Arm Attachment Point"));
    LeftArmAttachmentPoint->SetupAttachment(RootComponent);
    RightLegAttachmentPoint = CreateDefaultSubobject<USceneComponent>(TEXT("Right Leg Attachment Point"));
    RightLegAttachmentPoint->SetupAttachment(RootComponent);
    LeftLegAttachmentPoint = CreateDefaultSubobject<USceneComponent>(TEXT("Left Leg Attachment Point"));
    LeftLegAttachmentPoint->SetupAttachment(RootComponent);
}

void AGolemBody::EnableMesh()
{
    Mesh->SetVisibility(true);
    Mesh->SetComponentTickEnabled(true);
}

void AGolemBody::DisableMesh()
{
    Mesh->SetVisibility(false);
    Mesh->SetComponentTickEnabled(false);
}
