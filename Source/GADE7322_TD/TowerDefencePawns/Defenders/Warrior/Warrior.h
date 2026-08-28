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

public:
    virtual void StartAttack() override;

    virtual void EndAttack() override;

protected:
    virtual void DoOnSetActive(bool bActive) override;

public:
    const USkeletalMesh* GetMaleMesh() const;
    USkeletalMesh* GetMaleMesh();

    const USkeletalMesh* GetFemaleMesh() const;
    USkeletalMesh* GetFemaleMesh();

    const USkeletalMeshComponent* GetMesh() const;
    USkeletalMeshComponent* GetMesh();

    bool IsFemale() const;

    AWarrior& SetAttackTarget(const ATowerDefencePawn* Target)
    {
        CurrentAttackTarget = Target;
        return *this;
    }

    const ATowerDefencePawn* GetAttackTarget() const { return CurrentAttackTarget; }

    float GetAttackRadius() const { return AttackRadius; }

private:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
    USkeletalMesh* MaleMesh = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
    TSubclassOf<UAnimInstance> MaleAnimationBlueprint;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
    USkeletalMesh* FemaleMesh = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
    TSubclassOf<UAnimInstance> FemaleAnimationBlueprint;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    USkeletalMeshComponent* SkeletalMesh = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gender", meta = (AllowPrivateAccess = "true"))
    bool bIsFemale;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI", meta = (AllowPrivateAccess = "true"))
    const ATowerDefencePawn* CurrentAttackTarget;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI",
              meta = (AllowPrivateAccess = "true", Units = "Centimeters", ClampMin = "0.0", UIMin = "0.0"))
    float AttackRadius = 100.0f;
};
