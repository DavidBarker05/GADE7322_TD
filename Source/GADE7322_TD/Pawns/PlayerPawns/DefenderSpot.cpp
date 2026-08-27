#include "Pawns/PlayerPawns/DefenderSpot.h"

#include "Components/BoxComponent.h"
#include "Events/EventBus.h"
#include "Pawns/PlayerTroop.h"

ADefenderSpot::ADefenderSpot()
{
    PrimaryActorTick.bCanEverTick = false;

    BoxCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Collider"));
    BoxCollider->SetupAttachment(RootComponent);
    BoxCollider->SetBoxExtent(ColliderHalfExtents);
    BoxCollider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    BoxCollider->SetCollisionObjectType(ECC_WorldDynamic);
    BoxCollider->SetCollisionResponseToAllChannels(ECR_Ignore);
    BoxCollider->SetCollisionResponseToChannel(ECC_GameTraceChannel1 /* = MouseClick */, ECR_Block);
}

void ADefenderSpot::BeginPlay()
{
    Super::BeginPlay();
    SUBSCRIBE_TO_EVENTS();
}

void ADefenderSpot::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
    UNSUBSCRIBE_FROM_EVENTS();
}

void ADefenderSpot::OnEventReceived_Implementation(const FName& EventName, const TArray<FAny>& Params)
{
    if (EVENT_MATCHES(TEXT("DeathEvent"), 1) && PARAMS_ARE_VALID && PARAMS_ARE_CORRECT_TYPES(AActor))
    {
        if (const ATowerDefensePawn* DeadPawn = Params[0].Get<ATowerDefensePawn>())
        {
            if (!IsValid(DeadPawn) || !IsValid(CurrentDefender)) return;
            if (!DeadPawn->IsA<APlayerTroop>()) return;
            if (DeadPawn != CurrentDefender) return;
            GetWorld()->DestroyActor(CurrentDefender); // Or maybe should play death animation? Works for now
            CurrentDefender = nullptr;
        }
    }
}

bool ADefenderSpot::IsOccupied() const { return IsValid(CurrentDefender); }

void ADefenderSpot::PurchaseDefender(const TSubclassOf<APlayerTroop>& DefenderBlueprint)
{
    if (!IsValid(DefenderBlueprint) || IsValid(CurrentDefender)) return;
    CurrentDefender = GetWorld()->SpawnActor<APlayerTroop>(DefenderBlueprint, GetTransform());
    BROADCAST_EVENT(TEXT("PurchaseEvent"), CurrentDefender->GetCost());
}

void ADefenderSpot::SellDefender()
{
    if (!IsValid(CurrentDefender)) return;
    BROADCAST_EVENT(TEXT("SellEvent"), CurrentDefender->GetSellPrice());
    GetWorld()->DestroyActor(CurrentDefender);
    CurrentDefender = nullptr;
}
