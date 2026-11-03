#pragma once

#include "CoreMinimal.h"

#include "Components/ActorComponent.h"

#include "OutlineComponent.generated.h"

USTRUCT(BlueprintType)
struct FMeshOutlinePair
{
    GENERATED_BODY()

    FComponentReference GetReference() const { return ComponentReference; }

    const UMaterialInterface* GetOutlineMaterial() const { return OutlineMaterial; }
    UMaterialInterface* GetOutlineMaterial() { return OutlineMaterial; }

private:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mesh", meta = (AllowPrivateAccess = true))
    FComponentReference ComponentReference;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Outline", meta = (AllowPrivateAccess = true))
    UMaterialInterface* OutlineMaterial = nullptr;
};

UCLASS(ClassGroup = (TowerDefencePawn), meta = (BlueprintSpawnableComponent))
class GADE7322_TD_API UOutlineComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
    virtual void BindMaterials();

    UFUNCTION(BlueprintCallable)
    virtual void UnbindMaterials();

    UFUNCTION(BlueprintCallable)
    void EnableOutline();

    UFUNCTION(BlueprintCallable)
    void DisableOutline();

private:
    UPROPERTY(EditDefaultsOnly, Category = "MeshOutline", meta = (AllowPrivateAccess = true))
    TArray<FMeshOutlinePair> MeshOutlinePairs;

    UPROPERTY()
    TArray<UMaterialInstanceDynamic*> OutlineMaterials;
};
