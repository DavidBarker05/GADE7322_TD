#include "HealthComponent.h"

#include "EventBus.h"

UHealthComponent::UHealthComponent() { PrimaryComponentTick.bCanEverTick = false; }

void UHealthComponent::BeginPlay() { Super::BeginPlay(); }

void UHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                     FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

int UHealthComponent::GetCurrentHealth() const { return CurrentHealth; }

void UHealthComponent::SetCurrrentHealth(int32 Health)
{
    CurrentHealth = FMath::Clamp(Health, 0, MaxHealth);
}

int UHealthComponent::GetMaxHealth() const { return MaxHealth; }

void UHealthComponent::SetMaxHealth(int32 Health)
{
    MaxHealth = FMath::Max(0, Health);
    CurrentHealth = FMath::Clamp(CurrentHealth, 0, MaxHealth);
}

void UHealthComponent::TakeDamage(int32 Damage)
{
    CurrentHealth = FMath::Max(0, CurrentHealth - Damage);
    if (CurrentHealth == 0)
        BROADCAST_EVENT(TEXT("DeathEvent"), GetOwner());
}

void UHealthComponent::ReceiveHealth(int32 Health)
{
    CurrentHealth = FMath::Min(MaxHealth, CurrentHealth + Health);
}
