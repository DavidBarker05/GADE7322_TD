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

    bool IsTwoHanded() const { return bTwoHanded; }

    const USceneComponent* GetSecondHandPlacement() const { return SecondHandPlacement; }
    USceneComponent* GetSecondHandPlacement() { return SecondHandPlacement; }

    bool DoesMeshTick() const { return bMeshTicks; }

    UFUNCTION(BlueprintCallable)
    bool AttachToSkeleton(USkeletalMeshComponent* Skeleton, EWeaponHand Hand = EWeaponHand::Right);

private:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
    USceneComponent* Root;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
    UStaticMeshComponent* WeaponMesh;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
    bool bTwoHanded;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
    USceneComponent* SecondHandPlacement;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = true))
    FRotator AttachmentRotationOffset = FRotator::ZeroRotator;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = true))
    FVector AttachmentLocationOffset = FVector::ZeroVector;

    bool bMeshTicks;
};
