#include "TowerDefencePawns/Attackers/ProceduralEnemy/ProceduralParts/GolemBody.h"

AGolemBody::AGolemBody()
{
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
