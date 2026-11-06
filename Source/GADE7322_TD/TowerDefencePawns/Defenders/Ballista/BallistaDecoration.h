#pragma once

#include "TDCore.h"

#include "GameFramework/Actor.h"

#include "BallistaDecoration.generated.h"

class UHitFlashComponent;
class UOutlineComponent;

UCLASS(Abstract)
class ABallistaDecoration : public AActor
{
    GENERATED_BODY()

public:
    ABallistaDecoration();

    void SetVisible(bool bVisible);

    const UHitFlashComponent* GetHitFlashComponent() const { return HitFlashComponent; }
    UHitFlashComponent* GetHitFlashComponent() { return HitFlashComponent; }

    const UOutlineComponent* GetOutlineComponent() const { return OutlineComponent; }
    UOutlineComponent* GetOutlineComponent() { return OutlineComponent; }

private:
    void PopulateMeshes();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = true))
    UHitFlashComponent* HitFlashComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = true))
    UOutlineComponent* OutlineComponent;

    UPROPERTY()
    TArray<UStaticMeshComponent*> MeshComponents;
};
