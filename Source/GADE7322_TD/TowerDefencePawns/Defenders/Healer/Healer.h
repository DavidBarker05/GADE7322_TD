#pragma once

#include "CoreMinimal.h"

#include "TowerDefencePawns/Defenders/Defender.h"

#include "Healer.generated.h"

// TODO: Spell in hand when has target

UCLASS(Abstract)
class GADE7322_TD_API AHealer : public ADefender
{
    GENERATED_BODY()

public:
    AHealer();

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

    float GetHealRadius() const { return HealRadius; }
    float GetAttackRadius() const { return AttackRadius; }

private:
    bool IsOtherPawnFriendly(const ATowerDefencePawn* OtherPawn) const;

    void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

    void OnDeathMontageEnded(UAnimMontage* Montage, bool bInterrupted);

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Healer", meta = (AllowPrivateAccess = true))
    USceneComponent* SpellSpawnLocation;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = true))
    UAnimMontage* AttackMontage;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = true))
    UAnimMontage* HealMontage;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = true))
    UAnimMontage* DeathMontage;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Healer", meta = (AllowPrivateAccess = true))
    ATowerDefencePawn* CurrentTarget;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Healer", meta = (AllowPrivateAccess = true))
    bool bTargetIsFriendly = false;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Healer",
              meta = (AllowPrivateAccess = true, ClampMin = 0, UIMin = 0))
    int32 HealAmount = 0;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI",
              meta = (AllowPrivateAccess = true, ClampMin = 0.0, UIMin = 0.0, Units = "Centimeters"))
    float HealRadius = 200.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI",
              meta = (AllowPrivateAccess = true, ClampMin = 0.0, UIMin = 0.0, Units = "Centimeters"))
    float AttackRadius = 100.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI",
              meta = (AllowPrivateAccess = true, ClampMin = 0.0, UIMin = 0.0, Units = "Seconds"))
    float AttackCooldown = 0.5f;

    FTimerHandle AttackCooldownHandle;
};
