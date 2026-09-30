// ReSharper disable CppParameterMayBeConst
#pragma once

#include "CoreMinimal.h"

#include "Components/ActorComponent.h"
#include "PriorityQueue.h"

#include "DamageComponent.generated.h"

class UHealthComponent;

UCLASS(ClassGroup = (TowerDefencePawn), meta = (BlueprintSpawnableComponent))
class GADE7322_TD_API UDamageComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
    virtual void DamageOther(UHealthComponent* HealthComponent);

    UFUNCTION(BlueprintPure)
    int32 GetDamage() const { return FMath::RoundToInt32(Damage * GetDamageMultiplier()); }
    UFUNCTION(BlueprintCallable)
    void SetDamage(int32 InDamage) { Damage = InDamage; }

    float GetDamageMultiplier() const { return DamageBoosts.IsEmpty() ? 1.0f : DamageBoosts.Top(); }

    bool AddDamageBoost(float Multiplier)
    {
        DamageBoosts.Push(Multiplier);
        return DamageBoosts.Num() == 1;
    }

    bool RemoveDamageBoost(float Multiplier)
    {
        if (DamageBoosts.IsEmpty()) return false;
        TArray<float>& DamageBoostsContainer = DamageBoosts.GetContainer();
        for (int32 i = 0; i < DamageBoostsContainer.Num(); ++i)
        {
            if (DamageBoostsContainer[i] == Multiplier)
            {
                DamageBoostsContainer.HeapRemoveAt(i, TGreater<float>());
                break;
            }
        }
        return DamageBoosts.IsEmpty();
    }

protected:
    UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true, ClampMin = 0, UIMin = 0))
    int32 Damage = 0;

    TPriorityQueue<float> DamageBoosts;
    // Priority queue is something I learned on Leetcode
    // Basically like a queue, but it sorts elements when adding them so useful for getting the largest element
    // idk why unreal doesn't have one built in by default
};
