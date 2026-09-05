// ReSharper disable CppParameterMayBeConst
#include "TowerDefencePawns/Components/HealthComponent.h"

#include "CustomLog.h"
#include "Events/EventBus.h"

void UHealthComponent::TakeDamage(int32 Damage)
{
    if (bDead) return; // Don't keep taking damage and broadcasting events when dead
    CurrentHealth = FMath::Max(0, CurrentHealth - Damage);
    TD_LOG_INFO(TEXT("UHealthComponent::TakeDamage -> %s took %d damage, CurrentHealth = %d/%d"),
                GetOwner() ? *GetOwner()->GetName() : TEXT("INVALID"), Damage, CurrentHealth, MaxHealth);
    if (CurrentHealth == 0)
    {
        bAlive = false;
        bDead = true;
        TD_LOG_INFO(TEXT("UHealthComponent::TakeDamage -> %s died, broadcasting DeathEvent"),
                    GetOwner() ? *GetOwner()->GetName() : TEXT("INVALID"));
        BROADCAST_EVENT(TEXT("DeathEvent"), GetOwner());
    }
}

void UHealthComponent::ReceiveHealth(int32 Health) { CurrentHealth = FMath::Min(MaxHealth, CurrentHealth + Health); }
