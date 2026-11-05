#pragma once

#include "TDCore.h"

#include "Components/ActorComponent.h"
#include "TowerDefencePawns/Attackers/ProceduralEnemy/Animation/GolemAnimationPose.h"

#include "GolemAnimator.generated.h"

class ACharacter;
class AGolemArm;
class AGolemBody;
class AGolemLeg;

UENUM(BlueprintType)
enum class EGolemAnimationEase : uint8
{
    Linear,
    EaseIn,
    EaseOut,
    EaseInOut
};

USTRUCT(BlueprintType)
struct FGolemAttackKey
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack", meta = (Units = "Degrees"))
    float ArmPitch = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack",
              meta = (ClampMin = 0.0, UIMin = 0.0, Units = "Seconds"))
    float Duration = 0.2f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
    EGolemAnimationEase Ease = EGolemAnimationEase::EaseInOut;
};

DECLARE_MULTICAST_DELEGATE(FGolemAnimatorEvent);

UCLASS(ClassGroup = (Golem), meta = (BlueprintSpawnableComponent))
class GADE7322_TD_API UGolemAnimator : public UActorComponent
{
    GENERATED_BODY()

public:
    UGolemAnimator();

    virtual void TickComponent(float DeltaTime, ELevelTick TickType,
                               FActorComponentTickFunction* ThisTickFunction) override;

    void Setup(AGolemBody* Body, AGolemArm* RightArm, AGolemArm* LeftArm, AGolemLeg* RightLeg, AGolemLeg* LeftLeg);

    void Stop();

    void Freeze();

    void PlayAttack();

    bool IsAttacking() const { return bAttacking; }

    FGolemAnimatorEvent OnAttackHit;

    FGolemAnimatorEvent OnAttackFinished;

private:
    enum class EState : uint8
    {
        Idle,
        Walk,
        Attack
    };

    struct FJoint
    {
        TWeakObjectPtr<USceneComponent> Component;
        FVector BaseLocation = FVector::ZeroVector;
        FQuat BaseRotation = FQuat::Identity;
    };

    struct FParameters
    {
        float RightStrideAngle = 0.0f;
        float LeftStrideAngle = 0.0f;
        float RightBodyLowest = 0.0f;
        float RightBodyHighest = 0.0f;
        float LeftBodyLowest = 0.0f;
        float LeftBodyHighest = 0.0f;
        float RightArmIdleLowest = 0.0f;
        float RightArmIdleHighest = 0.0f;
        float LeftArmIdleLowest = 0.0f;
        float LeftArmIdleHighest = 0.0f;
    };

    static void CaptureJoint(FJoint& Joint, USceneComponent* Component);

    static void ApplyJoint(const FJoint& Joint, float Pitch, float OffsetZ);

    void Apply(const FGolemAnimationPose& Pose) const;

    void AdvanceAttack(float DeltaTime);

    float GetAttackDuration() const;

    float GetAttackHitTime() const;

    float ApplyEase(float Alpha, EGolemAnimationEase Ease) const;

    FGolemAnimationPose EvaluateIdle() const;

    FGolemAnimationPose EvaluateWalk() const;

    FGolemAnimationPose EvaluateAttack() const;

    UPROPERTY(EditAnywhere, Category = "Golem|Animation", meta = (ClampMin = 0.0, UIMin = 0.0, Units = "Seconds"))
    float BlendTime = 0.2f;

    UPROPERTY(EditAnywhere, Category = "Golem|Animation", meta = (ClampMin = 0.0, UIMin = 0.0, Units = "Seconds"))
    float AttackBlendTime = 0.08f;

    UPROPERTY(EditAnywhere, Category = "Golem|Animation|Walk", meta = (ClampMin = 0.0, UIMin = 0.0, Units = "cm/s"))
    float WalkSpeedThreshold = 5.0f;

    UPROPERTY(EditAnywhere, Category = "Golem|Animation|Walk", meta = (ClampMin = 0.0, UIMin = 0.0, Units = "Hertz"))
    float WalkCyclesPerSecond = 1.5f;

    UPROPERTY(EditAnywhere, Category = "Golem|Animation|Idle", meta = (ClampMin = 0.0, UIMin = 0.0, Units = "Hertz"))
    float IdleCyclesPerSecond = 0.4f;

    UPROPERTY(EditAnywhere, Category = "Golem|Animation|Attack", meta = (ClampMin = 1.0, UIMin = 1.0))
    float EaseExponent = 2.0f;

    UPROPERTY(EditAnywhere, Category = "Golem|Animation|Attack")
    TArray<FGolemAttackKey> AttackKeys;

    UPROPERTY(EditAnywhere, Category = "Golem|Animation|Attack", meta = (ClampMin = 0, UIMin = 0))
    int32 AttackHitKeyIndex = 3;

    bool bSetUp = false;
    bool bAttacking = false;
    bool bAttackHitFired = false;
    float AttackTime = 0.0f;
    float WalkPhase = 0.0f;
    float IdlePhase = 0.0f;

    EState State = EState::Idle;
    FGolemAnimationPose CurrentPose;
    FGolemAnimationPose BlendFrom;
    float BlendAlpha = 1.0f;

    TWeakObjectPtr<ACharacter> OwnerCharacter;
    FParameters Parameters;
    FJoint BodyJoint;
    FJoint RightArmJoint;
    FJoint LeftArmJoint;
    FJoint RightLegJoint;
    FJoint LeftLegJoint;
};
