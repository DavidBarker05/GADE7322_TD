#pragma once

#include "TDCore.h"

#include "GameFramework/Actor.h"

#include "GolemArm.generated.h"

class UHitFlashComponent;

UCLASS(Abstract)
class GADE7322_TD_API AGolemArm : public AActor
{
    GENERATED_BODY()

public:
    AGolemArm();

    const UStaticMeshComponent* GetMesh() const { return Mesh; }
    UStaticMeshComponent* GetMesh() { return Mesh; }

    UFUNCTION(BlueprintCallable, Category = "Golem|Part|Arm")
    virtual void EnableMesh();

    UFUNCTION(BlueprintCallable, Category = "Golem|Part|Arm")
    virtual void DisableMesh();

    const UHitFlashComponent* GetHitFlashComponent() const { return HitFlashComponent; }
    UHitFlashComponent* GetHitFlashComponent() { return HitFlashComponent; }

    int32 GetDamage() const { return Damage; }

private:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Golem|Part|Arm|Components",
              meta = (AllowPrivateAccess = true))
    UStaticMeshComponent* Mesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Golem|Part|Arm|Components",
              meta = (AllowPrivateAccess = true))
    UHitFlashComponent* HitFlashComponent;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Golem|Part|Arm|Damage",
              meta = (AllowPrivateAccess = true, ClampMin = 0, UIMin = 0))
    int32 Damage;
};
