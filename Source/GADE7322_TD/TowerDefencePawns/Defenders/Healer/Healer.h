#pragma once

#include "CoreMinimal.h"

#include "TowerDefencePawns/Defenders/Defender.h"

#include "Healer.generated.h"

class AFireballProjectile;
class ATowerDefenceGameMode;
class UAnimInstance;
class UNiagaraComponent;

USTRUCT(BlueprintType)
struct FHealerSkeletalMesh
{
    GENERATED_BODY()

    const USkeletalMesh* GetSkeletalMesh() const { return SkeletalMesh; }
    USkeletalMesh* GetSkeletalMesh() { return SkeletalMesh; }

    float GetHalfHeight() const { return HalfHeight; }

    TSubclassOf<UAnimInstance> GetAnimationBlueprint() const { return AnimationBlueprint; }

    const UAnimMontage* GetHealMontage() const { return HealMontage; }
    UAnimMontage* GetHealMontage() { return HealMontage; }

    const UAnimMontage* GetAttackMontage() const { return AttackMontage; }
    UAnimMontage* GetAttackMontage() { return AttackMontage; }

    const UAnimMontage* GetDeathMontage() const { return DeathMontage; }
    UAnimMontage* GetDeathMontage() { return DeathMontage; }

private:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Healer|Mesh", meta = (AllowPrivateAccess = true))
    USkeletalMesh* SkeletalMesh = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Healer|Mesh",
              meta = (AllowPrivateAccess = true, ClampMin = 0.0, UIMin = 0.0, Units = "Centimeters"))
    float HalfHeight = 0.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Healer|Animation", meta = (AllowPrivateAccess = true))
    TSubclassOf<UAnimInstance> AnimationBlueprint;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Healer|Animation|Montage",
              meta = (AllowPrivateAccess = true))
    UAnimMontage* HealMontage = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Healer|Animation|Montage",
              meta = (AllowPrivateAccess = true))
    UAnimMontage* AttackMontage = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Healer|Animation|Montage",
              meta = (AllowPrivateAccess = true))
    UAnimMontage* DeathMontage = nullptr;
};

USTRUCT(BlueprintType)
struct FHealerLevel
{
    GENERATED_BODY()

    const FHealerSkeletalMesh& GetHealerSkeletalMesh() const { return HealerSkeletalMesh; }
    FHealerSkeletalMesh& GetHealerSkeletalMesh() { return HealerSkeletalMesh; }

    float GetHealRadius() const { return HealRadius; }

    float GetAttackRadius() const { return AttackRadius; }

    float GetAttackNoThrowingRadius() const { return AttackNoThrowingRadius; }

    float GetAttackCooldown() const { return AttackCooldown; }

    int32 GetHealAmount() const { return HealAmount; }

private:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Healer|Upgrades|Mesh",
              meta = (AllowPrivateAccess = true))
    FHealerSkeletalMesh HealerSkeletalMesh;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Healer|Upgrades|AI",
              meta = (AllowPrivateAccess = true, ClampMin = 0.0, UIMin = 0.0, Units = "Centimeters"))
    float HealRadius = 0.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Healer|Upgrades|AI",
              meta = (AllowPrivateAccess = true, ClampMin = 0.0, UIMin = 0.0, Units = "Centimeters"))
    float AttackRadius = 0.0f;

    // The radius that if the enemy is within this distance then throwing a spell would be
    // inconsistent so just damage them instead
    UPROPERTY(EditDefaultsOnly, Category = "Healer|Upgrades",
              meta = (AllowPrivateAccess = true, ClampMin = 0.0, UIMin = 0.0, Units = "Centimeters"))
    float AttackNoThrowingRadius = 0.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Healer|Upgrades|AI",
              meta = (AllowPrivateAccess = true, ClampMin = 0.0, UIMin = 0.0, Units = "Seconds"))
    float AttackCooldown = 0.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Healer|Upgrades",
              meta = (AllowPrivateAccess = true, ClampMin = 0, UIMin = 0))
    int32 HealAmount = 0;
};

UCLASS(Abstract)
class GADE7322_TD_API AHealer : public ADefender
{
    GENERATED_BODY()

public:
    AHealer();

    virtual void BeginPlay() override;

    virtual void Tick(float DeltaTime) override;

    virtual void StartAttack() override;

    virtual void Heal(ATowerDefencePawn* Other);

    virtual void Attack(ATowerDefencePawn* Other) override;

    virtual void EndAttack() override;

    virtual void OnDeath(TFunction<void()>&& Func) override;

protected:
    virtual void DoOnSetActive(bool bActive) override;

    virtual void DoUpdatePerceptionOnTeamChange() override;

    virtual void DoOnLevelChanged(int32 Level) override;

public:
    const ATowerDefencePawn* GetCurrentTarget() const { return CurrentTarget; }
    ATowerDefencePawn* GetCurrentTarget() { return CurrentTarget; }

    bool IsCurrentTargetFriendly() const { return bTargetIsFriendly; }

    UFUNCTION(BlueprintCallable)
    void SetCurrentTarget(ATowerDefencePawn* NewTarget);

    float GetHealRadius() const { return HealRadius; }
    float GetAttackRadius() const { return AttackRadius; }

    virtual float GetPrimaryRadius() const override { return HealRadius; }
    virtual float GetSecondaryRadius() const override { return AttackRadius; }

    FHealerLevel GetHealerBaseLevel() const { return HealerBaseLevel; }
    FHealerLevel GetHealerFirstUpgrade() const { return HealerFirstUpgrade; }
    FHealerLevel GetHealerSecondUpgrade() const { return HealerSecondUpgrade; }

    UFUNCTION(BlueprintPure)
    FHealerLevel GetHealerCurrentLevel() const;

private:
    static void EnableSpellBall(UNiagaraComponent* Spell);
    static void DisableSpellBall(UNiagaraComponent* Spell);

    void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

    void OnDeathMontageEnded(UAnimMontage* Montage, bool bInterrupted);

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Healer|Spell", meta = (AllowPrivateAccess = true))
    USceneComponent* SpellSpawnLocation;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Healer|Spell", meta = (AllowPrivateAccess = true))
    UNiagaraComponent* HealSpellBall;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Healer|Spell", meta = (AllowPrivateAccess = true))
    UNiagaraComponent* FireSpellBall;

    UPROPERTY(EditDefaultsOnly, Category = "Healer|AI",
              meta = (AllowPrivateAccess = true, ClampMin = 0.0, UIMin = 0.0, ClampMax = 16.667, UIMax = 16.667,
                      Units = "Hertz"))
    float SpellDistanceCheckUpdateFrequency = 5.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Healer|Spell", meta = (AllowPrivateAccess = true))
    UStaticMeshComponent* SpellBook;

    float TimeSinceLastVisionUpdate = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Healer|Spell",
              meta = (AllowPrivateAccess = true, ClampMin = 1.0, UIMin = 1.0))
    float SpellShowRadiusMultiplier = 2.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Healer|Spell", meta = (AllowPrivateAccess = true))
    bool bIsHoldingSpell = false;

    UPROPERTY(EditDefaultsOnly, Category = "Healer|Spell", meta = (AllowPrivateAccess = true))
    TSubclassOf<AFireballProjectile> FireballClass;

    UPROPERTY(EditDefaultsOnly, Category = "Healer|Spell",
              meta = (AllowPrivateAccess = true, ClampMin = 0.0, UIMin = 0.0, Units = "cm/s"))
    float FireballSpeed = 2000.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Healer", meta = (AllowPrivateAccess = true))
    ATowerDefencePawn* CurrentTarget;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Healer", meta = (AllowPrivateAccess = true))
    bool bTargetIsFriendly = false;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Healer|Upgrades", meta = (AllowPrivateAccess = true))
    FHealerLevel HealerBaseLevel;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Healer|Upgrades", meta = (AllowPrivateAccess = true))
    FHealerLevel HealerFirstUpgrade;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Healer|Upgrades", meta = (AllowPrivateAccess = true))
    FHealerLevel HealerSecondUpgrade;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Healer|AI",
              meta = (AllowPrivateAccess = true, Units = "Centimeters"))
    float HealRadius;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Healer|AI",
              meta = (AllowPrivateAccess = true, Units = "Centimeters"))
    float AttackRadius;

    // The radius that if the enemy is within this distance then throwing a spell would be
    // inconsistent so just damage them instead
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Healer",
              meta = (AllowPrivateAccess = true, Units = "Centimeters"))
    float AttackNoThrowingRadius;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Healer|AI",
              meta = (AllowPrivateAccess = true, Units = "Seconds"))
    float AttackCooldown;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Healer", meta = (AllowPrivateAccess = true))
    int32 HealAmount;

    UPROPERTY()
    ATowerDefenceGameMode* TowerDefenceGameMode;

    FTimerHandle AttackCooldownHandle;
};
