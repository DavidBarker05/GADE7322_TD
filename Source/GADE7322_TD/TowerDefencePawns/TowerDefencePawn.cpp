#include "TowerDefencePawns/TowerDefencePawn.h"

#include "EventListener.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "TowerDefencePawns/Components/DamageComponent.h"
#include "TowerDefencePawns/Components/HealthComponent.h"

ATowerDefencePawn::ATowerDefencePawn()
{
    PrimaryActorTick.bCanEverTick = true;
    HealthComponent = CreateDefaultSubobject<UHealthComponent>("Health Component");
    DamageComponent = CreateDefaultSubobject<UDamageComponent>("Damage Component");
    StimuliSourceComponent = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>("Stimuli Source Component");
}

void ATowerDefencePawn::BeginPlay()
{
    Super::BeginPlay();
    if (!bIsPawnActive) StimuliSourceComponent->UnregisterFromPerceptionSystem();
}

ATowerDefencePawn& ATowerDefencePawn::SetPawnActive(bool bActive)
{
    bIsPawnActive = bActive;
    if (bActive) StimuliSourceComponent->RegisterWithPerceptionSystem();
    else StimuliSourceComponent->UnregisterFromPerceptionSystem();
    DoOnSetActive(bActive);
    return *this;
}

void ATowerDefencePawn::Tick(float DeltaTime) { Super::Tick(DeltaTime); }

void ATowerDefencePawn::Attack(ATowerDefencePawn* Other)
{
    if (IsValid(Other)) DamageComponent->DamageOther(Other->HealthComponent);
}

void ATowerDefencePawn::OnDeath(TFunction<void()>&& Func)
{
    DestroyDelegate = MoveTemp(Func);
    OnDeathComplete();
}

void ATowerDefencePawn::OnDeathComplete()
{
    if (DestroyDelegate) DestroyDelegate();
}

const UHealthComponent* ATowerDefencePawn::GetHealthComponent() const { return HealthComponent; }

UHealthComponent* ATowerDefencePawn::GetHealthComponent() { return HealthComponent; }

const UDamageComponent* ATowerDefencePawn::GetDamageComponent() const { return DamageComponent; }

UDamageComponent* ATowerDefencePawn::GetDamageComponent() { return DamageComponent; }

const FName& ATowerDefencePawn::GetPawnDisplayName() const { return PawnDisplayName; }
