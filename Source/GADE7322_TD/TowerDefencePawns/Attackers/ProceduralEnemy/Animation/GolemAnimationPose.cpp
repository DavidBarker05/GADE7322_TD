#include "TowerDefencePawns/Attackers/ProceduralEnemy/Animation/GolemAnimationPose.h"

FGolemAnimationPose FGolemAnimationPose::Lerp(const FGolemAnimationPose& A, const FGolemAnimationPose& B, float Alpha)
{
    FGolemAnimationPose Result;
    Result.BodyOffset = FMath::Lerp(A.BodyOffset, B.BodyOffset, Alpha);
    Result.RightLegPitch = FMath::Lerp(A.RightLegPitch, B.RightLegPitch, Alpha);
    Result.LeftLegPitch = FMath::Lerp(A.LeftLegPitch, B.LeftLegPitch, Alpha);
    Result.RightArmPitch = FMath::Lerp(A.RightArmPitch, B.RightArmPitch, Alpha);
    Result.LeftArmPitch = FMath::Lerp(A.LeftArmPitch, B.LeftArmPitch, Alpha);
    Result.RightArmOffset = FMath::Lerp(A.RightArmOffset, B.RightArmOffset, Alpha);
    Result.LeftArmOffset = FMath::Lerp(A.LeftArmOffset, B.LeftArmOffset, Alpha);
    return Result;
}
