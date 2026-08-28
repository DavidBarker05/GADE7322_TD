// ReSharper disable CppParameterMayBeConst
#include "TowerDefencePawns/Components/HealthComponent.h"

#include "Events/EventBus.h"

void UHealthComponent::TakeDamage(int32 Damage)
{
    CurrentHealth = FMath::Max(0, CurrentHealth - Damage);
    if (CurrentHealth == 0) BROADCAST_EVENT(TEXT("DeathEvent"), GetOwner());
}

void UHealthComponent::ReceiveHealth(int32 Health) { CurrentHealth = FMath::Min(MaxHealth, CurrentHealth + Health); }
