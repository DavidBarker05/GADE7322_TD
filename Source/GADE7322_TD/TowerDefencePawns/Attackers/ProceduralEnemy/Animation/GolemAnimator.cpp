#include "TowerDefencePawns/Attackers/ProceduralEnemy/Animation/GolemAnimator.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TowerDefencePawns/Attackers/ProceduralEnemy/ProceduralParts/GolemArm.h"
#include "TowerDefencePawns/Attackers/ProceduralEnemy/ProceduralParts/GolemBody.h"
#include "TowerDefencePawns/Attackers/ProceduralEnemy/ProceduralParts/GolemLeg.h"

namespace
{
    constexpr float Tau = 2.0f * UE_PI;
}

UGolemAnimator::UGolemAnimator()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.bStartWithTickEnabled = false;

    AttackKeys = {
        {-15.0f, 0.18f, EGolemAnimationEase::EaseInOut}, // Anticipation
        {165.0f, 0.35f, EGolemAnimationEase::EaseOut  }, // Raise
        {165.0f, 0.12f, EGolemAnimationEase::Linear   }, // Hold
        {20.0f,  0.14f, EGolemAnimationEase::EaseIn   }, // Strike
        {-12.0f, 0.08f, EGolemAnimationEase::EaseOut  }, // Overshoot
        {0.0f,   0.4f,  EGolemAnimationEase::EaseInOut}, // Wind down
    };
}

void UGolemAnimator::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    if (!bSetUp) return;

    if (bAttacking)
    {
        AdvanceAttack(DeltaTime);
        if (!bSetUp) return;
    }
    const ACharacter* Character = OwnerCharacter.Get();
    const float Speed = Character ? Character->GetVelocity().Size2D() : 0.0f;
    const EState Desired = bAttacking ? EState::Attack : (Speed > WalkSpeedThreshold ? EState::Walk : EState::Idle);
    if (Desired != State)
    {
        BlendFrom = CurrentPose;
        BlendAlpha = 0.0f;
        State = Desired;
    }
    FGolemAnimationPose Target;
    switch (State)
    {
        case EState::Walk:
            {
                const float MaxSpeed = Character ? Character->GetCharacterMovement()->MaxWalkSpeed : 0.0f;
                const float SpeedScale =
                    MaxSpeed > UE_KINDA_SMALL_NUMBER ? FMath::Clamp(Speed / MaxSpeed, 0.25f, 1.5f) : 1.0f;
                WalkPhase = FMath::Fmod(WalkPhase + DeltaTime * Tau * WalkCyclesPerSecond * SpeedScale, Tau);
            } // Have to create a scope because of how switch statements get compiled :/
            Target = EvaluateWalk();
            break;
        case EState::Attack:
            Target = EvaluateAttack();
            break;
        default:
            IdlePhase = FMath::Fmod(IdlePhase + DeltaTime * Tau * IdleCyclesPerSecond, Tau);
            Target = EvaluateIdle();
            break;
    }

    const float BlendDuration = State == EState::Attack ? AttackBlendTime : BlendTime;
    BlendAlpha =
        BlendDuration > UE_KINDA_SMALL_NUMBER ? FMath::Min(1.0f, BlendAlpha + DeltaTime / BlendDuration) : 1.0f;
    CurrentPose = FGolemAnimationPose::Lerp(BlendFrom, Target, FMath::SmoothStep(0.0f, 1.0f, BlendAlpha));
    Apply(CurrentPose);
}

void UGolemAnimator::Setup(AGolemBody* Body, AGolemArm* RightArm, AGolemArm* LeftArm, AGolemLeg* RightLeg,
                           AGolemLeg* LeftLeg)
{
    Stop();
    if (!IsValid(Body) || !IsValid(RightArm) || !IsValid(LeftArm) || !IsValid(RightLeg) || !IsValid(LeftLeg)) return;
    OwnerCharacter = Cast<ACharacter>(GetOwner());
    CaptureJoint(BodyJoint, Body->GetRootComponent());
    CaptureJoint(RightArmJoint, Body->GetRightArmAttachmentPoint());
    CaptureJoint(LeftArmJoint, Body->GetLeftArmAttachmentPoint());
    CaptureJoint(RightLegJoint, Body->GetRightLegAttachmentPoint());
    CaptureJoint(LeftLegJoint, Body->GetLeftLegAttachmentPoint());
    Parameters.RightStrideAngle = RightLeg->GetMaxStrideAngle();
    Parameters.LeftStrideAngle = LeftLeg->GetMaxStrideAngle();
    Parameters.RightBodyLowest = RightLeg->GetBodyLowestOffset();
    Parameters.RightBodyHighest = RightLeg->GetBodyHighestOffset();
    Parameters.LeftBodyLowest = LeftLeg->GetBodyLowestOffset();
    Parameters.LeftBodyHighest = LeftLeg->GetBodyHighestOffset();
    Parameters.RightArmIdleLowest = RightArm->GetIdleLowestOffset();
    Parameters.RightArmIdleHighest = RightArm->GetIdleHighestOffset();
    Parameters.LeftArmIdleLowest = LeftArm->GetIdleLowestOffset();
    Parameters.LeftArmIdleHighest = LeftArm->GetIdleHighestOffset();
    WalkPhase = FMath::FRandRange(0.0f, Tau);
    IdlePhase = FMath::FRandRange(0.0f, Tau);
    State = EState::Idle;
    CurrentPose = FGolemAnimationPose();
    BlendFrom = CurrentPose;
    BlendAlpha = 1.0f;
    bAttacking = false;
    bAttackHitFired = false;
    AttackTime = 0.0f;
    bSetUp = true;
    SetComponentTickEnabled(true);
}

void UGolemAnimator::Stop()
{
    if (bSetUp) Apply(FGolemAnimationPose());
    bSetUp = false;
    bAttacking = false;
    BodyJoint = FJoint();
    RightArmJoint = FJoint();
    LeftArmJoint = FJoint();
    RightLegJoint = FJoint();
    LeftLegJoint = FJoint();
    SetComponentTickEnabled(false);
}

void UGolemAnimator::Freeze() { SetComponentTickEnabled(false); }

void UGolemAnimator::PlayAttack()
{
    if (bAttacking) return;
    if (!bSetUp || AttackKeys.IsEmpty())
    {
        OnAttackHit.Broadcast();
        OnAttackFinished.Broadcast();
        return;
    }
    bAttacking = true;
    bAttackHitFired = false;
    AttackTime = 0.0f;
}

void UGolemAnimator::CaptureJoint(FJoint& Joint, USceneComponent* Component)
{
    Joint.Component = Component;
    if (!Component) return;
    Joint.BaseLocation = Component->GetRelativeLocation();
    Joint.BaseRotation = Component->GetRelativeRotation().Quaternion();
}

void UGolemAnimator::ApplyJoint(const FJoint& Joint, float Pitch, float OffsetZ)
{
    USceneComponent* Component = Joint.Component.Get();
    if (!Component) return;
    Component->SetRelativeLocationAndRotation(Joint.BaseLocation + FVector(0.0f, 0.0f, OffsetZ),
                                              Joint.BaseRotation * FRotator(Pitch, 0.0f, 0.0f).Quaternion());
}

void UGolemAnimator::Apply(const FGolemAnimationPose& Pose) const
{
    ApplyJoint(BodyJoint, 0.0f, Pose.BodyOffset);
    ApplyJoint(RightArmJoint, Pose.RightArmPitch, Pose.RightArmOffset);
    ApplyJoint(LeftArmJoint, Pose.LeftArmPitch, Pose.LeftArmOffset);
    ApplyJoint(RightLegJoint, Pose.RightLegPitch, 0.0f);
    ApplyJoint(LeftLegJoint, Pose.LeftLegPitch, 0.0f);
}

void UGolemAnimator::AdvanceAttack(float DeltaTime)
{
    AttackTime += DeltaTime;
    if (!bAttackHitFired && AttackTime >= GetAttackHitTime())
    {
        bAttackHitFired = true;
        OnAttackHit.Broadcast();
        if (!bSetUp || !bAttacking) return;
    }
    if (AttackTime >= GetAttackDuration())
    {
        bAttacking = false;
        OnAttackFinished.Broadcast();
    }
}

float UGolemAnimator::GetAttackDuration() const
{
    float Total = 0.0f;
    for (const FGolemAttackKey& Key : AttackKeys) Total += Key.Duration;
    return Total;
}

float UGolemAnimator::GetAttackHitTime() const
{
    if (AttackKeys.IsEmpty()) return 0.0f;
    const int32 HitKey = FMath::Clamp(AttackHitKeyIndex, 0, AttackKeys.Num() - 1);
    float Time = 0.0f;
    for (int32 i = 0; i <= HitKey; ++i) Time += AttackKeys[i].Duration;
    return Time;
}

float UGolemAnimator::ApplyEase(float Alpha, EGolemAnimationEase Ease) const
{
    switch (Ease)
    {
        case EGolemAnimationEase::EaseIn:
            return FMath::InterpEaseIn(0.0f, 1.0f, Alpha, EaseExponent);
        case EGolemAnimationEase::EaseOut:
            return FMath::InterpEaseOut(0.0f, 1.0f, Alpha, EaseExponent);
        case EGolemAnimationEase::EaseInOut:
            return FMath::InterpEaseInOut(0.0f, 1.0f, Alpha, EaseExponent);
        default:
            return Alpha;
    }
}

FGolemAnimationPose UGolemAnimator::EvaluateIdle() const
{
    const float Alpha = 0.5f + 0.5f * FMath::Sin(IdlePhase);
    FGolemAnimationPose Pose;
    Pose.RightArmOffset = FMath::Lerp(Parameters.RightArmIdleLowest, Parameters.RightArmIdleHighest, Alpha);
    Pose.LeftArmOffset = FMath::Lerp(Parameters.LeftArmIdleLowest, Parameters.LeftArmIdleHighest, Alpha);
    return Pose;
}

FGolemAnimationPose UGolemAnimator::EvaluateWalk() const
{
    const float Swing = FMath::Sin(WalkPhase);
    FGolemAnimationPose Pose;
    Pose.RightLegPitch = Parameters.RightStrideAngle * Swing;
    Pose.LeftLegPitch = -Parameters.LeftStrideAngle * Swing;
    const float Alpha = 0.5f + 0.5f * FMath::Cos(2.0f * WalkPhase);
    const float RightBody = FMath::Lerp(Parameters.RightBodyLowest, Parameters.RightBodyHighest, Alpha);
    const float LeftBody = FMath::Lerp(Parameters.LeftBodyLowest, Parameters.LeftBodyHighest, Alpha);
    Pose.BodyOffset = 0.5f * (RightBody + LeftBody);
    return Pose;
}

FGolemAnimationPose UGolemAnimator::EvaluateAttack() const
{
    float Remaining = AttackTime;
    float PreviousPitch = 0.0f;
    float Pitch = 0.0f;
    bool bFinished = true;
    for (const auto& [ArmPitch, Duration, Ease] : AttackKeys)
    {
        if (Remaining <= Duration)
        {
            const float Alpha = Duration > UE_KINDA_SMALL_NUMBER ? Remaining / Duration : 1.0f;
            Pitch = FMath::Lerp(PreviousPitch, ArmPitch, ApplyEase(Alpha, Ease));
            bFinished = false;
            break;
        }
        Remaining -= Duration;
        PreviousPitch = ArmPitch;
    }
    if (bFinished) Pitch = PreviousPitch;
    FGolemAnimationPose Pose;
    Pose.RightArmPitch = Pitch;
    Pose.LeftArmPitch = Pitch;
    return Pose;
}
