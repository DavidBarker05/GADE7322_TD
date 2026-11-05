#pragma once

#include "TDCore.h"

#include "GameFramework/Actor.h"

#include "GolemBody.generated.h"

class UHitFlashComponent;

UCLASS(Abstract)
class GADE7322_TD_API AGolemBody : public AActor
{
    GENERATED_BODY()

public:
    AGolemBody();

    const UStaticMeshComponent* GetMesh() const { return Mesh; }
    UStaticMeshComponent* GetMesh() { return Mesh; }

    UFUNCTION(BlueprintCallable, Category = "Golem|Part|Body")
    virtual void EnableMesh();

    UFUNCTION(BlueprintCallable, Category = "Golem|Part|Body")
    virtual void DisableMesh();

    const UHitFlashComponent* GetHitFlashComponent() const { return HitFlashComponent; }
    UHitFlashComponent* GetHitFlashComponent() { return HitFlashComponent; }

    const USceneComponent* GetHeadAttachmentPoint() const { return HeadAttachmentPoint; }
    USceneComponent* GetHeadAttachmentPoint() { return HeadAttachmentPoint; }

    const USceneComponent* GetRightArmAttachmentPoint() const { return RightArmAttachmentPoint; }
    USceneComponent* GetRightArmAttachmentPoint() { return RightArmAttachmentPoint; }

    const USceneComponent* GetLeftArmAttachmentPoint() const { return LeftArmAttachmentPoint; }
    USceneComponent* GetLeftArmAttachmentPoint() { return LeftArmAttachmentPoint; }

    const USceneComponent* GetRightLegAttachmentPoint() const { return RightLegAttachmentPoint; }
    USceneComponent* GetRightLegAttachmentPoint() { return RightLegAttachmentPoint; }

    const USceneComponent* GetLeftLegAttachmentPoint() const { return LeftLegAttachmentPoint; }
    USceneComponent* GetLeftLegAttachmentPoint() { return LeftLegAttachmentPoint; }

    int32 GetHealth() const { return Health; }

private:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = true))
    UStaticMeshComponent* Mesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = true))
    UHitFlashComponent* HitFlashComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components|Golem|Part|Body|AttachPoint|Head",
              meta = (AllowPrivateAccess = true))
    USceneComponent* HeadAttachmentPoint;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components|Golem|Part|Body|AttachPoint|Arm",
              meta = (AllowPrivateAccess = true))
    USceneComponent* RightArmAttachmentPoint;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components|Golem|Part|Body|AttachPoint|Arm",
              meta = (AllowPrivateAccess = true))
    USceneComponent* LeftArmAttachmentPoint;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components|Golem|Part|Body|AttachPoint|Leg",
              meta = (AllowPrivateAccess = true))
    USceneComponent* RightLegAttachmentPoint;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components|Golem|Part|Body|AttachPoint|Leg",
              meta = (AllowPrivateAccess = true))
    USceneComponent* LeftLegAttachmentPoint;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Golem|Part|Body|Health",
              meta = (AllowPrivateAccess = true, ClampMin = 0, UIMin = 0))
    int32 Health;
};
