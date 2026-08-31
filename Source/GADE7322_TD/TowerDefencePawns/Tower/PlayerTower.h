#pragma once

#include "CoreMinimal.h"
#include "Net/RepLayout.h"

#include "TowerDefencePawns/TowerDefencePawn.h"

#include "PlayerTower.generated.h"

class UStaticMeshComponent;

UCLASS(Abstract)
class GADE7322_TD_API APlayerTower : public ATowerDefencePawn
{
    GENERATED_BODY()

public:
    APlayerTower();

    virtual void StartAttack() override;

    // Resetting attack might break stuff? Especially if order keeps changing even if the elements are the same

    APlayerTower& SetAttackTargets(const TArray<ATowerDefencePawn*>& Targets)
    {
        if (Targets.Num() != 3) return *this;
        AttackTargets = Targets;
        CanAttackTarget = {true, true, true};
        for (FTimerHandle& Handle : TimerHandles) GetWorldTimerManager().ClearTimer(Handle);
        return *this;
    }

    APlayerTower& SetAttackTarget(uint32 Index, ATowerDefencePawn* Target)
    {
        if (Index >= 3 || !IsValid(Target)) return *this;
        if (AttackTargets.Contains(Target)) return *this;
        AttackTargets[Index] = Target;
        CanAttackTarget[Index] = true;
        GetWorldTimerManager().ClearTimer(TimerHandles[Index]);
        return *this;
    }

    TArray<ATowerDefencePawn*> GetAttackTargets() const { return AttackTargets; }

    const ATowerDefencePawn* GetAttackTarget(uint32 Index) { return Index < 3 ? AttackTargets[Index] : nullptr; }

private:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = true))
    UStaticMeshComponent* TowerMesh;

    UPROPERTY(BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = true))
    TArray<ATowerDefencePawn*> AttackTargets;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI",
              meta = (AllowPrivateAccess = true, ClampMin = 0.0, UIMin = 0.0, Units = "Centimeters"))
    float AttackRadius = 200.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI",
              meta = (AllowPrivateAccess = true, ClampMin = 0.0, UIMin = 0.0, Units = "Seconds"))
    float AttackCooldown = 1.0f;

    TArray<bool, TFixedAllocator<3>> CanAttackTarget = {true, true, true};

    TArray<FTimerHandle, TFixedAllocator<3>> TimerHandles;
};
