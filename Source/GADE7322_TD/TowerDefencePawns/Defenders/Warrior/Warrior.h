#pragma once

#include "CoreMinimal.h"

#include "TowerDefencePawns/Defenders/Defender.h"

#include "Warrior.generated.h"

class AWarriorShield;
class AWeapon;
class UAnimInstance;
class UChildActorComponent;
class USkeletalMesh;
class USkeletalMeshComponent;

USTRUCT(BlueprintType)
struct FWarriorSkeletalMesh
{
    GENERATED_BODY()

    const USkeletalMesh* GetSkeletalMesh() const { return SkeletalMesh; }
    USkeletalMesh* GetSkeletalMesh() { return SkeletalMesh; }

    float GetHalfHeight() const { return HalfHeight; }

    TSubclassOf<UAnimInstance> GetAnimationBlueprint() const { return AnimationBlueprint; }

    const UAnimMontage* GetAttackMontage() const { return AttackMontage; }
    UAnimMontage* GetAttackMontage() { return AttackMontage; }

    const UAnimMontage* GetDeathMontage() const { return DeathMontage; }
    UAnimMontage* GetDeathMontage() { return DeathMontage; }

private:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Warrior|Mesh", meta = (AllowPrivateAccess = true))
    USkeletalMesh* SkeletalMesh = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Warrior|Mesh",
              meta = (AllowPrivateAccess = true, ClampMin = 0.0, UIMin = 0.0, Units = "Centimeters"))
    float HalfHeight = 0.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Warrior|Animation", meta = (AllowPrivateAccess = true))
    TSubclassOf<UAnimInstance> AnimationBlueprint;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Warrior|Animation|Montage",
              meta = (AllowPrivateAccess = true))
    UAnimMontage* AttackMontage = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Warrior|Animation|Montage",
              meta = (AllowPrivateAccess = true))
    UAnimMontage* DeathMontage = nullptr;
};

USTRUCT(BlueprintType)
struct FWarriorLevel
{
    GENERATED_BODY()

    float GetAttackRadius() const { return AttackRadius; }

    float GetAttackCooldown() const { return AttackCooldown; }

private:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Warrior|Upgrades|AI",
              meta = (AllowPrivateAccess = true, ClampMin = 0.0, UIMin = 0.0, Units = "Centimeters"))
    float AttackRadius = 0.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Warrior|Upgrades|AI",
              meta = (AllowPrivateAccess = true, ClampMin = 0.0, UIMin = 0.0, Units = "Seconds"))
    float AttackCooldown = 0.0f;
};

UCLASS(Abstract)
class GADE7322_TD_API AWarrior : public ADefender
{
    GENERATED_BODY()

public:
    AWarrior();

    virtual void StartAttack() override;

    virtual void EndAttack() override;

    virtual void OnDeath(TFunction<void()>&& Func) override;

protected:
    virtual void DoOnSetActive(bool bActive) override;

    virtual void DoUpdatePerceptionOnTeamChange() override;

    virtual void DoOnLevelChanged(int32 Level) override;

public:
    FWarriorSkeletalMesh GetMaleWarrior() const { return MaleWarrior; }
    FWarriorSkeletalMesh GetFemaleWarrior() const { return FemaleWarrior; }
    FWarriorSkeletalMesh GetFinalWarrior() const { return FinalWarrior; }

    UFUNCTION(BlueprintPure)
    FWarriorSkeletalMesh GetCurrentWarrior() const;

    FWarriorLevel GetWarriorBaseLevel() const { return WarriorBaseLevel; }
    FWarriorLevel GetWarriorFirstUpgrade() const { return WarriorFirstUpgrade; }
    FWarriorLevel GetWarriorSecondUpgrade() const { return WarriorSecondUpgrade; }

    UFUNCTION(BlueprintPure)
    FWarriorLevel GetWarriorCurrentLevel() const;

    AWarrior& SetAttackTarget(ATowerDefencePawn* Target)
    {
        CurrentAttackTarget = Target;
        return *this;
    }

    const ATowerDefencePawn* GetAttackTarget() const { return CurrentAttackTarget; }

    float GetAttackRadius() const { return AttackRadius; }

    virtual float GetPrimaryRadius() const override { return AttackRadius; }

    const AWeapon* GetWeapon() const;
    AWeapon* GetWeapon();

    UFUNCTION(BlueprintPure, Category = "Warrior|Weapon", meta = (DisplayName = "Get Weapon"))
    AWeapon* Blueprint_GetWeapon() const;

private:
    UFUNCTION(BlueprintPure, Category = "Warrior|Weapon")
    AWeapon* GetOneHandSword() const;

    UFUNCTION(BlueprintPure, Category = "Warrior|Shield")
    AWarriorShield* GetShield() const;

    UFUNCTION(BlueprintPure, Category = "Warrior|Weapon")
    AWeapon* GetTwoHandSword() const;

    void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

    void OnDeathMontageEnded(UAnimMontage* Montage, bool bInterrupted);

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Warrior", meta = (AllowPrivateAccess = true))
    FWarriorSkeletalMesh MaleWarrior;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Warrior", meta = (AllowPrivateAccess = true))
    FWarriorSkeletalMesh FemaleWarrior;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Warrior", meta = (AllowPrivateAccess = true))
    FWarriorSkeletalMesh FinalWarrior;

    UPROPERTY(VisibleAnywhere, Category = "Weapon", meta = (AllowPrivateAccess = true))
    UChildActorComponent* OneHandWeapon;

    UPROPERTY(VisibleAnywhere, Category = "Shield", meta = (AllowPrivateAccess = true))
    UChildActorComponent* WarriorShield;

    UPROPERTY(VisibleAnywhere, Category = "Weapon", meta = (AllowPrivateAccess = true))
    UChildActorComponent* TwoHandWeapon;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI", meta = (AllowPrivateAccess = true))
    ATowerDefencePawn* CurrentAttackTarget;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Warrior|Upgrades", meta = (AllowPrivateAccess = true))
    FWarriorLevel WarriorBaseLevel;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Warrior|Upgrades", meta = (AllowPrivateAccess = true))
    FWarriorLevel WarriorFirstUpgrade;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Warrior|Upgrades", meta = (AllowPrivateAccess = true))
    FWarriorLevel WarriorSecondUpgrade;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Warrior|AI",
              meta = (AllowPrivateAccess = true, Units = "Centimeters"))
    float AttackRadius;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Warrior|AI",
              meta = (AllowPrivateAccess = true, Units = "Seconds"))
    float AttackCooldown;

    bool bIsFemale;

    FTimerHandle AttackCooldownHandle;
};
