#pragma once

#include "CoreMinimal.h"

#include "GolemAnimationPose.generated.h"

USTRUCT(BlueprintType)
struct FGolemAnimationPose
{
    GENERATED_BODY()

    FGolemAnimationPose() = default;

    UPROPERTY(BlueprintReadWrite)
    float BodyOffset = 0.0f;

    UPROPERTY(BlueprintReadWrite)
    float RightLegPitch = 0.0f;
    UPROPERTY(BlueprintReadWrite)
    float LeftLegPitch = 0.0f;

    UPROPERTY(BlueprintReadWrite)
    float RightArmPitch = 0.0f;
    UPROPERTY(BlueprintReadWrite)
    float LeftArmPitch = 0.0f;

    UPROPERTY(BlueprintReadWrite)
    float RightArmOffset = 0.0f;
    UPROPERTY(BlueprintReadWrite)
    float LeftArmOffset = 0.0f;

    static FGolemAnimationPose Lerp(const FGolemAnimationPose& A, const FGolemAnimationPose& B, float Alpha);
};
