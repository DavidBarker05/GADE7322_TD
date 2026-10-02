#include "TowerDefencePawns/Components/HealthComponent.h"

#include "Events/EventBus.h"
#include "Kismet/GameplayStatics.h"
#include "TowerDefenceGameMode.h"
#include "TowerDefencePawns/Components/HitFlashComponent.h"
#include "TowerDefencePawns/Defenders/Defender.h"
#include "TowerDefencePawns/TowerDefencePawn.h"

void UHealthComponent::BeginPlay()
{
    Super::BeginPlay();
    bOwnerIsDefender = static_cast<bool>(GetOwner<ADefender>());
    TowerDefenceGameMode = Cast<ATowerDefenceGameMode>(UGameplayStatics::GetGameMode(this));
}

void UHealthComponent::TakeDamage(int32 Damage)
{
    if (Damage <= 0) return;
    if (bDead) return; // Don't keep taking damage and broadcasting events when dead
    const int32 PreviousHealth = CurrentHealth;
    CurrentHealth = FMath::Max(0, CurrentHealth - Damage);
    if (bOwnerIsDefender && TowerDefenceGameMode)
        TowerDefenceGameMode->DefenderLostHealth(PreviousHealth - CurrentHealth);
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
    if (Health <= 0) return;
    if (bDead) return;
    const int32 PreviousHealth = CurrentHealth;
    CurrentHealth = FMath::Min(MaxHealth, CurrentHealth + Health);
    if (bOwnerIsDefender && TowerDefenceGameMode)
        TowerDefenceGameMode->DefenderGainedHealth(CurrentHealth - PreviousHealth);
    if (ATowerDefencePawn* TDP = GetOwner<ATowerDefencePawn>()) TDP->UpdateHealthDisplay();
}
