#pragma once

#include "TDCore.h"

#include "GameFramework/Actor.h"

#include "GolemLeg.generated.h"

class UHitFlashComponent;

UCLASS(Abstract)
class GADE7322_TD_API AGolemLeg : public AActor
{
    GENERATED_BODY()

public:
    AGolemLeg();

    const UStaticMeshComponent* GetMesh() const { return Mesh; }
    UStaticMeshComponent* GetMesh() { return Mesh; }

    UFUNCTION(BlueprintCallable, Category = "Golem|Part|Leg")
    virtual void EnableMesh();

    UFUNCTION(BlueprintCallable, Category = "Golem|Part|Leg")
    virtual void DisableMesh();

    const UHitFlashComponent* GetHitFlashComponent() const { return HitFlashComponent; }
    UHitFlashComponent* GetHitFlashComponent() { return HitFlashComponent; }

    float GetSpeed() const { return Speed; }

private:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = true))
    UStaticMeshComponent* Mesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = true))
    UHitFlashComponent* HitFlashComponent;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Golem|Part|Leg|Speed",
              meta = (AllowPrivateAccess = true, ClampMin = 0.0, UIMin = 0.0, Units = "cm/s"))
    float Speed;
};
