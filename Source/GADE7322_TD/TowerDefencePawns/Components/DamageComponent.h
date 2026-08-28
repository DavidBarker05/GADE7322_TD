// ReSharper disable CppParameterMayBeConst
#pragma once

#include "CoreMinimal.h"

#include "Components/ActorComponent.h"

#include "DamageComponent.generated.h"

class UHealthComponent;

UCLASS(ClassGroup = (TowerDefencePawn), meta = (BlueprintSpawnableComponent))
class GADE7322_TD_API UDamageComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
    virtual void DamageOther(UHealthComponent* HealthComponent);

    int32 GetDamage() const { return Damage; }
    void SetDamage(int32 InDamage) { Damage = InDamage; }

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true, ClampMin = 0, UIMin = 0))
    int32 Damage = 0;
};
