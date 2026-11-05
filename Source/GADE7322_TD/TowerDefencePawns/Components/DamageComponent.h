#pragma once

#include "TDCore.h"

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

    UFUNCTION(BlueprintCallable)
    void SetStrengthMultiplier(float Multiplier);

    UFUNCTION(BlueprintCallable)
    void RemoveStrengthMultiplier();

    UFUNCTION(BlueprintPure)
    float GetDamageMultiplier() const;

    UFUNCTION(BlueprintCallable)
    bool AddDamageBoost(float Multiplier);

    UFUNCTION(BlueprintCallable)
    bool RemoveDamageBoost(float Multiplier);

    UFUNCTION(BlueprintPure)
    int32 GetDamage() const;

    UFUNCTION(BlueprintCallable)
    void SetDamage(int32 InDamage);

protected:
    UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true, ClampMin = 0, UIMin = 0))
    int32 Damage = 0;

    bool bDoStrengthMultiplier = false;
    float StrengthMultiplier = 1.0f;

    TPriorityQueue<float> DamageBoosts;
    // Priority queue is something I learned on Leetcode
    // Basically like a queue, but it sorts elements when adding them so useful for getting the largest element
    // idk why unreal doesn't have one built in by default
};
