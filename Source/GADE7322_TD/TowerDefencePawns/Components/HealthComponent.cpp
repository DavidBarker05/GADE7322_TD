#include "TowerDefencePawns/Components/HealthComponent.h"

#include "Events/EventBus.h"

void UHealthComponent::TakeDamage(int32 Damage)
{
    if (Damage <= 0) return;
    if (bDead) return; // Don't keep taking damage and broadcasting events when dead
    const int32 PreviousHealth = CurrentHealth;
    CurrentHealth = FMath::Max(0, CurrentHealth - Damage);
    OnHealthChanged.Broadcast();
    OnHealthLost.Broadcast();
    OnHealthLostValue.Broadcast(PreviousHealth - CurrentHealth);
    if (CurrentHealth == 0)
    {
        bAlive = false;
        bDead = true;
        BROADCAST_EVENT(TEXT("DeathEvent"), GetOwner());
    }
}

void UHealthComponent::ReceiveHealth(int32 Health)
{
    if (Health <= 0) return;
    if (bDead) return;
    const int32 PreviousHealth = CurrentHealth;
    CurrentHealth = FMath::Min(MaxHealth, CurrentHealth + Health);
    OnHealthChanged.Broadcast();
    OnHealthGained.Broadcast();
    OnHealthGainedValue.Broadcast(CurrentHealth - PreviousHealth);
}
