#pragma once

#include "TDCore.h"

#include "TowerDefencePawns/Attackers/ProceduralEnemy/ProceduralParts/GolemPart.h"

#include "GolemBody.generated.h"

UCLASS(Abstract)
class GADE7322_TD_API AGolemBody : public AGolemPart
{
    GENERATED_BODY()

public:
    AGolemBody();

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
    int32 Health = 0;
};
