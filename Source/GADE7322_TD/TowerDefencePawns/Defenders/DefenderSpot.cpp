#include "TowerDefencePawns/Defenders/DefenderSpot.h"

#include "Components/BoxComponent.h"
#include "Events/EventBus.h"
#include "TowerDefencePawns/Defenders/Defender.h"
#include "TowerDefencePawns/TowerDefencePawnFactory.h"

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
    UNSUBSCRIBE_FROM_EVENTS();
    Super::EndPlay(EndPlayReason);
}

void ADefenderSpot::OnEventReceived_Implementation(const FName& EventName, const TArray<FAny>& Params)
{
    if (EVENT_MATCHES(TEXT("DeathEvent"), 1) && PARAMS_ARE_VALID && PARAMS_ARE_CORRECT_TYPES(ATowerDefencePawn))
    {
        if (const ATowerDefencePawn* DeadPawn = Params[0].Get<ATowerDefencePawn>())
        {
            if (!IsValid(DeadPawn) || !IsValid(CurrentDefender)) return;
            if (!DeadPawn->IsA<ADefender>()) return;
            if (DeadPawn != CurrentDefender) return;
            CurrentDefender->OnDeath(
                [this]() -> void
                {
                    if (TOWER_DEFENCE_PAWN_FACTORY_EXISTS)
                    {
                        DESTROY_PAWN(CurrentDefender);
                        CurrentDefender->SetPawnActive(false);
                        CurrentDefender = nullptr;
                    }
                });
        }
    }
}

bool ADefenderSpot::IsOccupied() const { return IsValid(CurrentDefender); }

void ADefenderSpot::PurchaseDefender(const TSubclassOf<ADefender>& DefenderBlueprint)
{
    if (!IsValid(DefenderBlueprint) || IsValid(CurrentDefender)) return;
    if (TOWER_DEFENCE_PAWN_FACTORY_EXISTS)
    {
        CurrentDefender = Cast<ADefender>(CREATE_PAWN(DefenderBlueprint, GetTransform()));
        CurrentDefender->SetPawnActive(true);
        BROADCAST_EVENT(TEXT("PurchaseEvent"), CurrentDefender->GetCost());
    }
}

void ADefenderSpot::SellDefender()
{
    if (!IsValid(CurrentDefender)) return;
    if (TOWER_DEFENCE_PAWN_FACTORY_EXISTS)
    {
        BROADCAST_EVENT(TEXT("SellEvent"), CurrentDefender->GetSellPrice());
        CurrentDefender->SetPawnActive(false);
        DESTROY_PAWN(CurrentDefender);
        CurrentDefender = nullptr;
    }
}
