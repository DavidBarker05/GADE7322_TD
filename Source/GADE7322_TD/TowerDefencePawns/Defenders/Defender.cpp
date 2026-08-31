#include "TowerDefencePawns/Defenders/Defender.h"

#include "Components/BoxComponent.h"

ADefender::ADefender()
{
    PawnDisplayName = TEXT("PlayerTroop");
    BoxCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Collider"));
    BoxCollider->SetupAttachment(RootComponent);
    BoxCollider->SetBoxExtent(ColliderHalfExtents);
    BoxCollider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    BoxCollider->SetCollisionObjectType(ECC_WorldDynamic);
    BoxCollider->SetCollisionResponseToAllChannels(ECR_Ignore);
    BoxCollider->SetCollisionResponseToChannel(ECC_GameTraceChannel1 /* = MouseClick */, ECR_Block);
}
