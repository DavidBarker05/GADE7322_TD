#pragma once

#include "CoreMinimal.h"

#include "TowerDefensePawns/Attackers/Attacker.h"

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

protected:
    virtual void DoOnSetActive() override;

    virtual void DoOnDeathComplete() override;

public:
    USkeletalMeshComponent* GetSkeletalMesh() { return SkeletalMesh; }
    const USkeletalMeshComponent* GetSkeletalMesh() const { return SkeletalMesh; }

    UFUNCTION(BlueprintCallable, Category = "AI")
    void SetAttackTarget(const ATowerDefensePawn* Target) { CurrentAttackTarget = Target; }

    const ATowerDefensePawn* GetAttackTarget() const { return CurrentAttackTarget; }

    float GetAttackRadius() const { return AttackRadius; }

private:
    UPROPERTY(EditDefaultsOnly, Category = "Meshes", meta = (AllowPrivateAccess = "true"))
    TArray<USkeletalMesh*> PossibleMeshes;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    USkeletalMeshComponent* SkeletalMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI", meta = (AllowPrivateAccess = "true"))
    const ATowerDefensePawn* CurrentAttackTarget;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI",
              meta = (AllowPrivateAccess = "true", ClampMin = "0.0", UIMin = "0.0"))
    float AttackRadius = 100.0f;
};
