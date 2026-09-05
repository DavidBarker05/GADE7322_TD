#pragma once

#include "CoreMinimal.h"

#include "Components/ActorComponent.h"
#include "Engine/EngineTypes.h"

#include "HitFlashComponent.generated.h"

UCLASS(ClassGroup = (TowerDefencePawn), meta = (BlueprintSpawnableComponent))
class GADE7322_TD_API UHitFlashComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
    virtual void BindMaterials();

    UFUNCTION(BlueprintCallable)
    virtual void UnbindMaterials();

    UFUNCTION(BlueprintCallable)
    virtual void DoFlash();

protected:
    UFUNCTION()
    virtual void EndFlash();

    FTimerHandle FlashHandle;

private:
    // The mesh components to flash for the AActor that owns this component
    UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
    TArray<FComponentReference> MeshComponents;

    UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
    FLinearColor FlashColor = FLinearColor::White;

    UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true, ClampMin = 0.0, UIMin = 0.0))
    float FlashIntensity = 1.0f;

    UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true, ClampMin = 0.0, UIMin = 0.0, Units = "Seconds"))
    float FlashDuration = 0.1f;

    UPROPERTY()
    TArray<UMaterialInstanceDynamic*> FlashMaterials;
};
