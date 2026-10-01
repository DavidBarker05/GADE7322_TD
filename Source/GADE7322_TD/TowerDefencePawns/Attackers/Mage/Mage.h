#pragma once

#include "CoreMinimal.h"

#include "TowerDefencePawns/Attackers/Attacker.h"

#include "Mage.generated.h"

class ADamageSpellProjectile;
class UNiagaraComponent;

UCLASS(Abstract)
class GADE7322_TD_API AMage : public AAttacker
{
    GENERATED_BODY()

public:
    AMage();

    virtual void BeginPlay() override;

    virtual void Tick(float DeltaTime) override;

    virtual void StartAttack() override;

    virtual void Attack(ATowerDefencePawn* Other) override;

    virtual void EndAttack() override;

    virtual void OnDeath(TFunction<void()>&& Func) override;

protected:
    virtual void DoOnSetActive(bool bActive) override;

    virtual void DoUpdatePerceptionOnTeamChange() override;

public:
    const ATowerDefencePawn* GetCurrentTarget() const { return CurrentTarget; }
    ATowerDefencePawn* GetCurrentTarget() { return CurrentTarget; }

    bool IsCurrentTargetFriendly() const { return bTargetIsFriendly; }

    UFUNCTION(BlueprintCallable)
    void SetCurrentTarget(ATowerDefencePawn* NewTarget);

    float GetBoostRadius() const { return BoostRadius; }
    float GetAttackRadius() const { return AttackRadius; }

private:
    bool IsOtherPawnFriendly(const ATowerDefencePawn* OtherPawn) const;

    void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

    void OnBoostStartMontageEnded(UAnimMontage* Montage, bool bInterrupted);

    void OnDeathMontageEnded(UAnimMontage* Montage, bool bInterrupted);

    void StopBoosting();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spell", meta = (AllowPrivateAccess = true))
    USceneComponent* SpellSpawnLocation;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spell", meta = (AllowPrivateAccess = true))
    UNiagaraComponent* BoostSpellBall;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spell", meta = (AllowPrivateAccess = true))
    UNiagaraComponent* DamageSpellBall;

    UPROPERTY(EditDefaultsOnly, Category = "AI",
              meta = (AllowPrivateAccess = true, ClampMin = 0.0, UIMin = 0.0, ClampMax = 16.667, UIMax = 16.667,
                      Units = "Hertz"))
    float SpellDistanceCheckUpdateFrequency = 5.0f;

    float TimeSinceLastVisionUpdate = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Spell", meta = (AllowPrivateAccess = true, ClampMin = 1.0, UIMin = 1.0))
    float SpellShowRadiusMultiplier = 2.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Spell", meta = (AllowPrivateAccess = true))
    bool bIsHoldingSpell = false;

    UPROPERTY(EditDefaultsOnly, Category = "Spell", meta = (AllowPrivateAccess = true))
    TSubclassOf<ADamageSpellProjectile> DamageSpellClass;

    UPROPERTY(EditDefaultsOnly, Category = "Spell",
              meta = (AllowPrivateAccess = true, ClampMin = 0.0, UIMin = 0.0, Units = "cm/s"))
    float DamageSpellSpeed = 2000.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = true))
    UAnimMontage* AttackMontage;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = true))
    UAnimMontage* BoostStartMontage;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = true))
    UAnimMontage* DeathMontage;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mage", meta = (AllowPrivateAccess = true))
    ATowerDefencePawn* CurrentTarget;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mage", meta = (AllowPrivateAccess = true))
    bool bTargetIsFriendly = false;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mage",
              meta = (AllowPrivateAccess = true, ClampMin = 1.0, UIMin = 1.0))
    float DamageBoostAmount = 1.2f;

    bool bBoostStarted = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mage", meta = (AllowPrivateAccess = true))
    bool bIsBoosting = false;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI",
              meta = (AllowPrivateAccess = true, ClampMin = 0.0, UIMin = 0.0, Units = "Centimeters"))
    float BoostRadius = 300.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI",
              meta = (AllowPrivateAccess = true, ClampMin = 0.0, UIMin = 0.0, Units = "Centimeters"))
    float AttackRadius = 200.0f;

    // The radius that if the enemy is within this distance then throwing a spell would be
    // inconsistent so just damage them instead
    UPROPERTY(EditDefaultsOnly, Category = "Mage",
              meta = (AllowPrivateAccess = true, ClampMin = 0.0, UIMin = 0.0, Units = "Centimeters"))
    float AttackNoThrowingRadius = 100.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI",
              meta = (AllowPrivateAccess = true, ClampMin = 0.0, UIMin = 0.0, Units = "Seconds"))
    float AttackCooldown = 0.5f;

    FTimerHandle AttackCooldownHandle;
};
