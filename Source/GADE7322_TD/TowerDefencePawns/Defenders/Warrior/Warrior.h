#pragma once

#include "CoreMinimal.h"

#include "TowerDefencePawns/Defenders/Defender.h"

#include "Warrior.generated.h"

class UAnimInstance;
class USkeletalMesh;
class USkeletalMeshComponent;

UCLASS(Abstract)
class GADE7322_TD_API AWarrior : public ADefender
{
    GENERATED_BODY()

public:
    AWarrior();

    virtual void BeginPlay() override;

    virtual void StartAttack() override;

    virtual void EndAttack() override;

protected:
    virtual void DoOnSetActive(bool bActive) override;

public:
    const USkeletalMeshComponent* GetMesh() const { return SkeletalMesh; }
    USkeletalMeshComponent* GetMesh() { return SkeletalMesh; }

    AWarrior& SetAttackTarget(const ATowerDefencePawn* Target)
    {
        CurrentAttackTarget = Target;
        return *this;
    }

    const ATowerDefencePawn* GetAttackTarget() const { return CurrentAttackTarget; }

    float GetAttackRadius() const { return AttackRadius; }

private:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mesh", meta = (AllowPrivateAccess = true))
    USkeletalMesh* MaleMesh = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = true))
    TSubclassOf<UAnimInstance> MaleAnimationBlueprint;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = true))
    UAnimMontage* MaleAttackMontage;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mesh", meta = (AllowPrivateAccess = true))
    USkeletalMesh* FemaleMesh = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = true))
    TSubclassOf<UAnimInstance> FemaleAnimationBlueprint;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = true))
    UAnimMontage* FemaleAttackMontage;

    UPROPERTY(BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = true))
    USkeletalMeshComponent* SkeletalMesh = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI", meta = (AllowPrivateAccess = true))
    const ATowerDefencePawn* CurrentAttackTarget;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI",
              meta = (AllowPrivateAccess = true, ClampMin = 0.0, UIMin = 0.0, Units = "Centimeters"))
    float AttackRadius = 100.0f;

    bool bIsFemale;
};
