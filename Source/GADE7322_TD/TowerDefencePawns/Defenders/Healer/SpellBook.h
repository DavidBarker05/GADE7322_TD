#pragma once

#include "TDCore.h"

#include "GameFramework/Actor.h"

#include "SpellBook.generated.h"

class UHitFlashComponent;
class UOutlineComponent;

UCLASS(Abstract)
class ASpellBook : public AActor
{
    GENERATED_BODY()

public:
    ASpellBook();

    const UStaticMeshComponent* GetMesh() const { return Mesh; }
    UStaticMeshComponent* GetMesh() { return Mesh; }

    const UHitFlashComponent* GetHitFlashComponent() const { return HitFlashComponent; }
    UHitFlashComponent* GetHitFlashComponent() { return HitFlashComponent; }

    const UOutlineComponent* GetOutlineComponent() const { return OutlineComponent; }
    UOutlineComponent* GetOutlineComponent() { return OutlineComponent; }

private:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = true))
    UStaticMeshComponent* Mesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = true))
    UHitFlashComponent* HitFlashComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = true))
    UOutlineComponent* OutlineComponent;
};
