// ReSharper disable CppParameterMayBeConst
#include "TowerDefencePawns/Components/HealthComponent.h"

#include "Events/EventBus.h"
#include "TowerDefencePawns/Components/HitFlashComponent.h"
#include "TowerDefencePawns/TowerDefencePawn.h"

void UHealthComponent::TakeDamage(int32 Damage)
{
    if (bDead) return; // Don't keep taking damage and broadcasting events when dead
    CurrentHealth = FMath::Max(0, CurrentHealth - Damage);
    if (ATowerDefencePawn* TDP = GetOwner<ATowerDefencePawn>())
    {
        TDP->UpdateHealthDisplay();
        TDP->GetHitFlashComponent()->DoFlash();
    }
    if (CurrentHealth == 0)
    {
        bAlive = false;
        bDead = true;
        BROADCAST_EVENT(TEXT("DeathEvent"), GetOwner());
    }
}

void UHealthComponent::ReceiveHealth(int32 Health)
{
    CurrentHealth = FMath::Min(MaxHealth, CurrentHealth + Health);
    if (ATowerDefencePawn* TDP = GetOwner<ATowerDefencePawn>()) TDP->UpdateHealthDisplay();
}
