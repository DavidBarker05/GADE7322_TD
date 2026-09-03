#include "TowerDefencePawns/Defenders/DefenderSpot.h"

#include "Components/BoxComponent.h"
#include "Events/EventBus.h"
#include "TDCollisionChannels.h"
#include "TowerDefencePawnAIControllerFactory.h"
#include "TowerDefencePawns/Defenders/Defender.h"
#include "TowerDefencePawns/TowerDefencePawnFactory.h"

ADefenderSpot::ADefenderSpot()
{
    PrimaryActorTick.bCanEverTick = false;
    Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    SetRootComponent(Root);
    BoxCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Collider"));
    BoxCollider->SetupAttachment(RootComponent);
    BoxCollider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    BoxCollider->SetCollisionObjectType(ECC_WorldDynamic);
    BoxCollider->SetCollisionResponseToAllChannels(ECR_Ignore);
    BoxCollider->SetCollisionResponseToChannel(MouseClickTraceChannel, ECR_Block);
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
    if (EVENT_MATCHES(TEXT("DeathEvent"), 1) && PARAMS_ARE_VALID && PARAMS_ARE_CORRECT_TYPES(ATowerDefencePawn*))
    {
        if (const ATowerDefencePawn* const* DeadPawnPtr = Params[0].Get<ATowerDefencePawn*>())
        {
            const ATowerDefencePawn* DeadPawn = *DeadPawnPtr;
            if (!IsValid(DeadPawn) || !IsValid(CurrentDefender)) return;
            if (!DeadPawn->IsA<ADefender>()) return;
            if (DeadPawn != CurrentDefender) return;
            CurrentDefender->OnDeath(
                [this]() -> void
                {
                    if (TOWER_DEFENCE_PAWN_FACTORY_EXISTS)
                    {
                        if (CurrentDefender->UseAIController()) UNPOSSESS_TOWER_DEFENCE_PAWN(CurrentDefender);
                        CurrentDefender->SetSpawnPoint(nullptr);
                        CurrentDefender->SetPawnActive(false);
                        DESTROY_PAWN(CurrentDefender);
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
        if (CurrentDefender->UseAIController())
            POSSESS_TOWER_DEFENCE_PAWN(CurrentDefender); // Will this cause issues here? We'll see
        CurrentDefender->SetSpawnPoint(this);
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
        if (CurrentDefender->UseAIController()) UNPOSSESS_TOWER_DEFENCE_PAWN(CurrentDefender);
        CurrentDefender->SetSpawnPoint(nullptr);
        CurrentDefender->SetPawnActive(false);
        DESTROY_PAWN(CurrentDefender);
        CurrentDefender = nullptr;
    }
}
