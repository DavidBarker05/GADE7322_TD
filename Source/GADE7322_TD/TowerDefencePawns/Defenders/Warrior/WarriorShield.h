#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Actor.h"

#include "WarriorShield.generated.h"

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

    bool DoesMeshTick() const { return bMeshTicks; }

    UFUNCTION(BlueprintCallable)
    bool AttachToSkeleton(USkeletalMeshComponent* Skeleton, EShieldArm Arm = EShieldArm::Left);

private:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
    USceneComponent* Root;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
    UStaticMeshComponent* ShieldMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
    bool bMeshTicks;
};
