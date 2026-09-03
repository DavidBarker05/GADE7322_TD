#include "TowerDefencePawns/Defenders/Defender.h"

#include "Components/BoxComponent.h"
#include "TDCollisionChannels.h"

ADefender::ADefender()
{
    PawnDisplayName = TEXT("PlayerTroop");
    BoxCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Collider"));
    BoxCollider->SetupAttachment(RootComponent);
    BoxCollider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    BoxCollider->SetCollisionObjectType(ECC_WorldDynamic);
    BoxCollider->SetCollisionResponseToAllChannels(ECR_Ignore);
    BoxCollider->SetCollisionResponseToChannel(MouseClickTraceChannel, ECR_Block);
}
