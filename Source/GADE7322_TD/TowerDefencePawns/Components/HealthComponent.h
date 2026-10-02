#pragma once

#include "CoreMinimal.h"

#include "Components/ActorComponent.h"

#include "HealthComponent.generated.h"

class ATowerDefenceGameMode;

UCLASS(ClassGroup = (TowerDefencePawn), meta = (BlueprintSpawnableComponent))
class GADE7322_TD_API UHealthComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    virtual void BeginPlay() override;

    int32 GetMaxHealth() const { return MaxHealth; }
    void SetMaxHealth(int32 Health)
    {
        MaxHealth = FMath::Max(0, Health);
        CurrentHealth = FMath::Clamp(CurrentHealth, 0, MaxHealth);
    }

    int32 GetCurrentHealth() const { return CurrentHealth; }
    void SetCurrentHealth(int32 Health) { CurrentHealth = FMath::Clamp(Health, 0, MaxHealth); }

    UFUNCTION(BlueprintCallable, Category = "Health")
    void TakeDamage(int32 Damage);

    UFUNCTION(BlueprintCallable, Category = "Health")
    void ReceiveHealth(int32 Health);

    UFUNCTION(BlueprintCallable, Category = "Health")
    void ResetHealth()
    {
        CurrentHealth = MaxHealth;
        bAlive = true;
        bDead = false;
    }

    UFUNCTION(BlueprintPure, Category = "Health")
    bool IsAlive() const { return bAlive; }

    UFUNCTION(BlueprintPure, Category = "Health")
    bool IsDead() const { return bDead; }

private:
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Health",
              meta = (AllowPrivateAccess = true, UIMin = 0, ClampMin = 0))
    int32 MaxHealth = 1;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Health",
              meta = (AllowPrivateAccess = true, UIMin = 0, ClampMin = 0))
    int32 CurrentHealth = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health", meta = (AllowPrivateAccess = true))
    bool bAlive = true;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health", meta = (AllowPrivateAccess = true))
    bool bDead = false;

    bool bOwnerIsDefender = false;

    UPROPERTY()
    ATowerDefenceGameMode* TowerDefenceGameMode;
    // If going to be updating game mode every time take damage then accessing directly
    // is better than broadcasting because of how often it happens
};
