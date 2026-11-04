#pragma once

#include "TDCore.h"

#include "GameFramework/Actor.h"

#include "WarriorShield.generated.h"

class UHitFlashComponent;
class UOutlineComponent;

UENUM(BlueprintType)
enum class EShieldArm : uint8
{
    Right,
    Left
};

UCLASS(Abstract)
class AWarriorShield : public AActor
{
    GENERATED_BODY()

public:
    AWarriorShield();

    const UStaticMeshComponent* GetMesh() const { return ShieldMesh; }
    UStaticMeshComponent* GetMesh() { return ShieldMesh; }

    const UHitFlashComponent* GetHitFlashComponent() const { return HitFlashComponent; }
    UHitFlashComponent* GetHitFlashComponent() { return HitFlashComponent; }

    const UOutlineComponent* GetOutlineComponent() const { return OutlineComponent; }
    UOutlineComponent* GetOutlineComponent() { return OutlineComponent; }

    bool DoesMeshTick() const { return bMeshTicks; }

    UFUNCTION(BlueprintCallable)
    bool AttachToSkeleton(USkeletalMeshComponent* Skeleton, EShieldArm Arm = EShieldArm::Left);

private:
    UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = true))
    USceneComponent* Root;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = true))
    UStaticMeshComponent* ShieldMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = true))
    UHitFlashComponent* HitFlashComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = true))
    UOutlineComponent* OutlineComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
    bool bMeshTicks;
};
