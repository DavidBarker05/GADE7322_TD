#include "HealthComponent.h"

#include "CustomStructs.h"
#include "EventBus.h"

UHealthComponent::UHealthComponent() { PrimaryComponentTick.bCanEverTick = false; }

void UHealthComponent::BeginPlay() { Super::BeginPlay(); }

void UHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                     FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

int UHealthComponent::GetCurrentHealth() { return CurrentHealth; }

int UHealthComponent::GetMaxHealth() { return MaxHealth; }

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
