// ReSharper disable CppParameterMayBeConst
#include "TowerDefencePawns/Components/HealthComponent.h"

#include "Events/EventBus.h"

void UHealthComponent::TakeDamage(int32 Damage)
{
    if (CurrentHealth == 0) return; // Don't keep taking damage and broadcasting events when dead
    CurrentHealth = FMath::Max(0, CurrentHealth - Damage);
    if (CurrentHealth == 0) BROADCAST_EVENT(TEXT("DeathEvent"), GetOwner());
}

void UHealthComponent::ReceiveHealth(int32 Health) { CurrentHealth = FMath::Min(MaxHealth, CurrentHealth + Health); }
