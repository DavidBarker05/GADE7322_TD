#pragma once

#include "TDCore.h"

#include "Components/ActorComponent.h"

#include "HealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHealthChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHealthLost);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHealthGained);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHealthLostValue, int32, Amount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHealthGainedValue, int32, Amount);

UCLASS(ClassGroup = (TowerDefencePawn), meta = (BlueprintSpawnableComponent))
class GADE7322_TD_API UHealthComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    int32 GetMaxHealth() const { return MaxHealth; }
    UHealthComponent& SetMaxHealth(int32 Health)
    {
        MaxHealth = FMath::Max(0, Health);
        CurrentHealth = FMath::Clamp(CurrentHealth, 0, MaxHealth);
        OnHealthChanged.Broadcast();
        return *this;
    }

    int32 GetCurrentHealth() const { return CurrentHealth; }
    UHealthComponent& SetCurrentHealth(int32 Health)
    {
        CurrentHealth = FMath::Clamp(Health, 0, MaxHealth);
        OnHealthChanged.Broadcast();
        return *this;
    }

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

    bool IsAlive() const { return bAlive; }

    bool IsDead() const { return bDead; }

    UPROPERTY(BlueprintAssignable)
    FOnHealthChanged OnHealthChanged;

    UPROPERTY(BlueprintAssignable)
    FOnHealthLost OnHealthLost;

    UPROPERTY(BlueprintAssignable)
    FOnHealthGained OnHealthGained;

    UPROPERTY(BlueprintAssignable)
    FOnHealthLostValue OnHealthLostValue;

    UPROPERTY(BlueprintAssignable)
    FOnHealthGainedValue OnHealthGainedValue;

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
};
