#include "TowerDefensePawns/TowerDefensePawn.h"

#include "EventListener.h"
#include "TowerDefensePawns/Components/DamageComponent.h"
#include "TowerDefensePawns/Components/HealthComponent.h"

ATowerDefensePawn::ATowerDefensePawn()
{
    PrimaryActorTick.bCanEverTick = true;
    HealthComponent = CreateDefaultSubobject<UHealthComponent>("Health Component");
    DamageComponent = CreateDefaultSubobject<UDamageComponent>("Damage Component");
}

void ATowerDefensePawn::BeginPlay() { Super::BeginPlay(); }

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
    DoOnDeathComplete();
    DestroyDelegate();
}

const UHealthComponent* ATowerDefensePawn::GetHealthComponent() const { return HealthComponent; }

UHealthComponent* ATowerDefensePawn::GetHealthComponent() { return HealthComponent; }

const UDamageComponent* ATowerDefensePawn::GetDamageComponent() const { return DamageComponent; }

UDamageComponent* ATowerDefensePawn::GetDamageComponent() { return DamageComponent; }

const FName& ATowerDefensePawn::GetPawnDisplayName() const { return PawnDisplayName; }
