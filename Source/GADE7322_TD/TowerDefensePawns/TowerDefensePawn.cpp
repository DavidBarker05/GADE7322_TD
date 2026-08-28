#include "TowerDefensePawns/TowerDefensePawn.h"

#include "EventListener.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "TowerDefensePawns/Components/DamageComponent.h"
#include "TowerDefensePawns/Components/HealthComponent.h"

ATowerDefensePawn::ATowerDefensePawn()
{
    PrimaryActorTick.bCanEverTick = true;
    HealthComponent = CreateDefaultSubobject<UHealthComponent>("Health Component");
    DamageComponent = CreateDefaultSubobject<UDamageComponent>("Damage Component");
    StimuliSourceComponent = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>("Stimuli Source Component");
}

void ATowerDefensePawn::BeginPlay()
{
    Super::BeginPlay();

    // Pooled pawns are spawned inactive and reused rather than destroyed. If a Blueprint has this
    // component's own auto-register turned on it would otherwise register at spawn time, before this
    // pawn has ever been made active, so force it back off here and let SetPawnActive() be the only
    // thing that turns detection on.
    if (!bIsPawnActive) StimuliSourceComponent->UnregisterFromPerceptionSystem();
}

ATowerDefensePawn& ATowerDefensePawn::SetPawnActive(bool bActive)
{
    bIsPawnActive = bActive;
    if (bActive) StimuliSourceComponent->RegisterWithPerceptionSystem();
    else StimuliSourceComponent->UnregisterFromPerceptionSystem();
    DoOnSetActive(bActive);
    return *this;
}

void ATowerDefensePawn::Tick(float DeltaTime) { Super::Tick(DeltaTime); }

void ATowerDefensePawn::Attack(ATowerDefensePawn* Other)
{
    if (Other && IsValid(Other)) DamageComponent->DamageOther(Other->HealthComponent);
}

void ATowerDefensePawn::OnDeath(TFunction<void()>&& Func)
{
    DestroyDelegate = MoveTemp(Func);
    OnDeathComplete();
}

void ATowerDefensePawn::OnDeathComplete()
{
    if (!DestroyDelegate) return;
    DestroyDelegate();
}

const UHealthComponent* ATowerDefensePawn::GetHealthComponent() const { return HealthComponent; }

UHealthComponent* ATowerDefensePawn::GetHealthComponent() { return HealthComponent; }

const UDamageComponent* ATowerDefensePawn::GetDamageComponent() const { return DamageComponent; }

UDamageComponent* ATowerDefensePawn::GetDamageComponent() { return DamageComponent; }

const FName& ATowerDefensePawn::GetPawnDisplayName() const { return PawnDisplayName; }
