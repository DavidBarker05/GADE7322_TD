#pragma once

#include "TDCore.h"

#include "Components/ActorComponent.h"
#include "Components/MeshComponent.h"
#include "Engine/EngineTypes.h"

#include "HitFlashComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMeshMaterialsAdded, UMeshComponent*, MeshComponent,
                                             TArray<UMaterialInstanceDynamic*>, MaterialInstances);

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

    UFUNCTION(BlueprintCallable)
    virtual void ForceStopFlash();

    UFUNCTION(BlueprintPure)
    virtual bool IsStillFlashing() const;

    UFUNCTION(BlueprintCallable)
    virtual bool PauseFlash();

    bool IsPaused() const { return bPaused; }

    float GetRemainingFlashDuration() const { return RemainingFlashDuration; }

    UFUNCTION(BlueprintCallable)
    virtual void SetRemainingFlashDuration(float InRemainingFlashDuration);

    UFUNCTION(BlueprintCallable)
    virtual void ResumeFlash();

protected:
    UFUNCTION()
    virtual void EndFlash();

public:
    UPROPERTY(BlueprintAssignable)
    FOnMeshMaterialsAdded OnMeshMaterialsAdded;

protected:
    FTimerHandle FlashHandle;

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
    bool bPaused = false;

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
    float RemainingFlashDuration;

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
