#pragma once

#include "CoreMinimal.h"

#include "Components/ActorComponent.h"

#include "HealthComponent.generated.h"
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class GADE7322_TD_API UHealthComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UHealthComponent();

protected:
    virtual void BeginPlay() override;

public:
    virtual void TickComponent(float DeltaTime, ELevelTick TickType,
                               FActorComponentTickFunction* ThisTickFunction) override;

public:
    int GetCurrentHealth() const;
    void SetCurrrentHealth(int32 Health);

    int GetMaxHealth() const;
    void SetMaxHealth(int32 Health);

    UFUNCTION(BlueprintCallable, Category = "Health")
    void TakeDamage(int32 Damage);

    UFUNCTION(BlueprintCallable, Category = "Health")
    void ReceiveHealth(int32 Health);

private:
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Health",
              meta = (AllowPrivateAccess = "true", DisplayName = "Max Health", UIMin = 0, ClampMin = 0))
    int32 MaxHealth = 1;

private:
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Health",
              meta = (AllowPrivateAccess = "true", DisplayName = "Max Health", UIMin = 0, ClampMin = 0))
    int32 CurrentHealth = 0;
};
