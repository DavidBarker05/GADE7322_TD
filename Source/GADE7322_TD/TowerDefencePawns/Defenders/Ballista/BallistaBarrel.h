#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Actor.h"

#include "BallistaBarrel.generated.h"

class UHitFlashComponent;
class UOutlineComponent;

// Ballista barrel for lack of a better name :/
UCLASS(Abstract)
class ABallistaBarrel : public AActor
{
    GENERATED_BODY()

public:
    ABallistaBarrel();

    const UStaticMeshComponent* GetMesh() const { return Mesh; }
    UStaticMeshComponent* GetMesh() { return Mesh; }

    const USceneComponent* GetMuzzle() const { return Muzzle; }
    USceneComponent* GetMuzzle() { return Muzzle; }

    const UHitFlashComponent* GetHitFlashComponent() const { return HitFlashComponent; }
    UHitFlashComponent* GetHitFlashComponent() { return HitFlashComponent; }

    const UOutlineComponent* GetOutlineComponent() const { return OutlineComponent; }
    UOutlineComponent* GetOutlineComponent() { return OutlineComponent; }

private:
    UPROPERTY()
    USceneComponent* Pivot;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = true))
    UStaticMeshComponent* Mesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = true))
    USceneComponent* Muzzle;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = true))
    UHitFlashComponent* HitFlashComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = true))
    UOutlineComponent* OutlineComponent;
};
