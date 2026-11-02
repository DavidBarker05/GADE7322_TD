#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Actor.h"

#include "BallistaDecoration.generated.h"

class UHitFlashComponent;

UCLASS(Abstract)
class ABallistaDecoration : public AActor
{
    GENERATED_BODY()

public:
    ABallistaDecoration();

    const UStaticMeshComponent* GetMesh() const { return Mesh; }
    UStaticMeshComponent* GetMesh() { return Mesh; }

    const UHitFlashComponent* GetHitFlashComponent() const { return HitFlashComponent; }
    UHitFlashComponent* GetHitFlashComponent() { return HitFlashComponent; }

private:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = true))
    UStaticMeshComponent* Mesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = true))
    UHitFlashComponent* HitFlashComponent;
};
