#pragma once

#include "TDCore.h"

#include "GameFramework/Actor.h"

#include "Weapon.generated.h"

UENUM(BlueprintType)
enum class EWeaponHand : uint8
{
    Right,
    Left
};

class UHitFlashComponent;
class USkeletalMeshComponent;

UCLASS(Abstract)
class GADE7322_TD_API AWeapon : public AActor
{
    GENERATED_BODY()

public:
    AWeapon();

    const UStaticMeshComponent* GetMesh() const { return WeaponMesh; }
    UStaticMeshComponent* GetMesh() { return WeaponMesh; }

    const UHitFlashComponent* GetHitFlashComponent() const { return HitFlashComponent; }
    UHitFlashComponent* GetHitFlashComponent() { return HitFlashComponent; }

    bool DoesMeshTick() const { return bMeshTicks; }

    UFUNCTION(BlueprintCallable)
    bool AttachToSkeleton(USkeletalMeshComponent* Skeleton, EWeaponHand Hand = EWeaponHand::Right);

private:
    UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = true))
    USceneComponent* Root;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = true))
    UStaticMeshComponent* WeaponMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = true))
    UHitFlashComponent* HitFlashComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
    bool bMeshTicks;
};
