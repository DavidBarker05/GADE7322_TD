#pragma once

#include "CoreMinimal.h"

#include "TowerDefencePawns/Attackers/Attacker.h"

#include "SkeletonPawn.generated.h"

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

protected:
    virtual void DoOnSetActive(bool bActive) override;

public:
    USkeletalMeshComponent* GetSkeletalMesh() { return SkeletalMesh; }
    const USkeletalMeshComponent* GetSkeletalMesh() const { return SkeletalMesh; }

    ASkeletonPawn& SetAttackTarget(const ATowerDefencePawn* Target)
    {
        CurrentAttackTarget = Target;
        return *this;
    }

    const ATowerDefencePawn* GetAttackTarget() const { return CurrentAttackTarget; }

    float GetAttackRadius() const { return AttackRadius; }

private:
    UPROPERTY(EditDefaultsOnly, Category = "Meshes", meta = (AllowPrivateAccess = true))
    TArray<USkeletalMesh*> PossibleMeshes;

    UPROPERTY(BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = true))
    USkeletalMeshComponent* SkeletalMesh;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = true))
    UAnimMontage* AttackMontage;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI", meta = (AllowPrivateAccess = true))
    const ATowerDefencePawn* CurrentAttackTarget;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI",
              meta = (AllowPrivateAccess = true, ClampMin = 0.0, UIMin = 0.0, Units = "Centimeters"))
    float AttackRadius = 100.0f;
};
