#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Actor.h"

#include "Weapon.generated.h"

UENUM(BlueprintType)
enum class EWeaponHand : uint8
{
    Right,
    Left
};

class USkeletalMeshComponent;

UCLASS(Abstract)
class GADE7322_TD_API AWeapon : public AActor
{
    GENERATED_BODY()

public:
    AWeapon();

    const UStaticMeshComponent* GetMesh() const { return WeaponMesh; }
    UStaticMeshComponent* GetMesh() { return WeaponMesh; }

    bool DoesMeshTick() const { return bMeshTicks; }

    UFUNCTION(BlueprintCallable)
    bool AttachToSkeleton(USkeletalMeshComponent* Skeleton, EWeaponHand Hand = EWeaponHand::Right);

private:
    // Pivot point for weapon
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = true))
    USceneComponent* Root;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
    UStaticMeshComponent* WeaponMesh;

    bool bMeshTicks;
};
