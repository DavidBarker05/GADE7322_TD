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
    UFUNCTION(BlueprintPure, Category = "Health")
    int GetCurrentHealth();

    UFUNCTION(BlueprintPure, Category = "Health")
    int GetMaxHealth();

    UFUNCTION(BlueprintCallable, Category = "Health")
    void TakeDamage(int32 Damage);

    UFUNCTION(BlueprintCallable, Category = "Health")
    void ReceiveHealth(int32 Health);

private:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health",
              meta = (AllowPrivateAccess = "true", DisplayName = "Max Health", UIMin = 0, ClampMin = 0))
    int32 MaxHealth = 1;

private:
    int32 CurrentHealth = 0;
};
