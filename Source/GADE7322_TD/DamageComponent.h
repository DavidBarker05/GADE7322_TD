#pragma once

#include "CoreMinimal.h"

#include "Components/ActorComponent.h"

#include "DamageComponent.generated.h"

class UHealthComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GADE7322_TD_API UDamageComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UDamageComponent();

protected:
    virtual void BeginPlay() override;

public:
    virtual void TickComponent(float DeltaTime, ELevelTick TickType,
                               FActorComponentTickFunction* ThisTickFunction) override;

public:
    UFUNCTION(BlueprintCallable)
    virtual void DamageOther(UHealthComponent* HealthComponent);

    int32 GetDamage() const;
    void SetDamage(int32 InDamage);

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess=true, ClampMin = "0", UIMin = "0"))
    int32 Damage = 0;
};
