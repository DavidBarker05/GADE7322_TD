#include "TowerDefencePawns/Components/HealthComponent.h"

#include "Events/EventBus.h"

void UHealthComponent::SetStrengthMultiplier(float Multiplier)
{
    if (Multiplier < 0.0f) return;
    bDoStrengthMultiplier = true;
    StrengthMultiplier = Multiplier;
}

void UHealthComponent::RemoveStrengthMultiplier()
{
    bDoStrengthMultiplier = false;
    StrengthMultiplier = 1.0f;
}

int32 UHealthComponent::GetMaxHealth() const
{
    return bDoStrengthMultiplier ? FMath::RoundToInt32(BaseMaxHealth * StrengthMultiplier) : BaseMaxHealth;
}

void UHealthComponent::SetBaseMaxHealth(int32 Health)
{
    BaseMaxHealth = FMath::Max(0, Health);
    CurrentHealth = FMath::Clamp(CurrentHealth, 0, GetMaxHealth());
    OnHealthChanged.Broadcast();
}

void UHealthComponent::SetCurrentHealth(int32 Health)
{
    CurrentHealth = FMath::Clamp(Health, 0, GetMaxHealth());
    OnHealthChanged.Broadcast();
}

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
    CurrentHealth = FMath::Min(GetMaxHealth(), CurrentHealth + Health);
    OnHealthChanged.Broadcast();
    OnHealthGained.Broadcast();
    OnHealthGainedValue.Broadcast(CurrentHealth - PreviousHealth);
}

void UHealthComponent::ResetHealth()
{
    CurrentHealth = GetMaxHealth();
    bAlive = true;
    bDead = false;
}
