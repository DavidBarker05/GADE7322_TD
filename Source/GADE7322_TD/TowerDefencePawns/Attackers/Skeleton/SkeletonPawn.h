#pragma once

#include "CoreMinimal.h"

#include "TowerDefencePawns/Attackers/Attacker.h"

#include "SkeletonPawn.generated.h"

class AWeapon;
class USkeletalMesh;
class USkeletalMeshComponent;

UCLASS(Abstract)
class GADE7322_TD_API ASkeletonPawn : public AAttacker
{
    GENERATED_BODY()

public:
    ASkeletonPawn();

    virtual void BeginPlay() override;

    virtual void StartAttack() override;

    virtual void EndAttack() override;

    virtual void OnDeath(TFunction<void()>&& Func) override;

protected:
    virtual void DoOnSetActive(bool bActive) override;

public:
    USkeletalMeshComponent* GetMesh() { return SkeletalMesh; }
    const USkeletalMeshComponent* GetMesh() const { return SkeletalMesh; }

    ASkeletonPawn& SetAttackTarget(ATowerDefencePawn* Target)
    {
        CurrentAttackTarget = Target;
        return *this;
    }

    const ATowerDefencePawn* GetAttackTarget() const { return CurrentAttackTarget; }

    float GetAttackRadius() const { return AttackRadius; }

    const AWeapon* GetSword() const { return Sword; }
    AWeapon* GetSword() { return Sword; }

private:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = true))
    USkeletalMeshComponent* SkeletalMesh;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = true))
    UAnimMontage* AttackMontage;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = true))
    UAnimMontage* DeathMontage;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI", meta = (AllowPrivateAccess = true))
    ATowerDefencePawn* CurrentAttackTarget;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = true))
    AWeapon* Sword;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI",
              meta = (AllowPrivateAccess = true, ClampMin = 0.0, UIMin = 0.0, Units = "Centimeters"))
    float AttackRadius = 100.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI",
              meta = (AllowPrivateAccess = true, ClampMin = 0.0, UIMin = 0.0, Units = "Seconds"))
    float AttackCooldown = 0.5f;

    FTimerHandle AttackCooldownHandle;
};
