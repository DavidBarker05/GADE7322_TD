#include "TowerDefencePawns/Attackers/AI/CommonTasks/MoveToPathPointTask.h"

#include "Components/CapsuleComponent.h"
#include "CustomLog.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "NavigationData.h"
#include "StateTreeExecutionContext.h"

namespace
{
    int32 UpdateNearestForwardPathIndex(AAttacker& Actor)
    {
        const TArray<FVector>& AllPoints = Actor.GetPathPoints();
        const int32 SearchStart = Actor.GetCurrentPathIndex();
        int32 NearestIndex = SearchStart;
        float NearestDistSq = TNumericLimits<float>::Max();
        for (int32 i = SearchStart; i < AllPoints.Num(); ++i)
        {
            const float DistSq = FVector::DistSquared2D(Actor.GetActorLocation(), AllPoints[i]);
            if (DistSq < NearestDistSq)
            {
                NearestDistSq = DistSq;
                NearestIndex = i;
            }
        }
        Actor.SetCurrentPathIndex(NearestIndex);
        return NearestIndex;
    }
} // namespace

EStateTreeRunStatus FMoveToPathPointTask::EnterState(FStateTreeExecutionContext& Context,
                                                     const FStateTreeTransitionResult& Transition) const
{
    auto& [AIController, Actor, StuckCheckLocation, StuckCheckTime, ConsecutiveStuckNudges] =
        Context.GetInstanceData(*this);
    StuckCheckTime = 0.0f;
    ConsecutiveStuckNudges = 0;
    if (Actor) StuckCheckLocation = Actor->GetActorLocation();
    if (!AIController)
    {
        TD_LOG_ERROR(TEXT("FMoveToPathPointTask::EnterState -> AI Controller context is nullptr"));
        return EStateTreeRunStatus::Failed;
    }
    if (!Actor)
    {
        TD_LOG_ERROR(TEXT("FMoveToPathPointTask::EnterState -> Actor context is nullptr"));
        return EStateTreeRunStatus::Failed;
    }
    const TArray<FVector>& AllPoints = Actor->GetPathPoints();
    if (AllPoints.IsEmpty())
    {
        TD_LOG_ERROR(TEXT("FMoveToPathPointTask::EnterState -> Attacker has no path"));
        return EStateTreeRunStatus::Failed;
    }

    const int32 StartIndex = UpdateNearestForwardPathIndex(*Actor);

    TArray<FVector> RemainingPoints(AllPoints.GetData() + StartIndex, AllPoints.Num() - StartIndex);
    if (RemainingPoints.Num() < 2) return EStateTreeRunStatus::Succeeded;

    UPathFollowingComponent* PFC = AIController->GetPathFollowingComponent();
    if (!PFC)
    {
        TD_LOG_ERROR(TEXT("FMoveToPathPointTask::EnterState -> %s missing PathFollowingComponent"), *Actor->GetName());
        return EStateTreeRunStatus::Failed;
    }

    const FNavPathSharedPtr NavPath = MakeShared<FNavigationPath>(RemainingPoints);
    FAIMoveRequest MoveReq(RemainingPoints.Last());
    MoveReq.SetAcceptanceRadius(Actor->GetPathTargetAcceptanceDistance());
    MoveReq.SetUsePathfinding(false); // We already built the route ourselves from the attacker's own procedural path
    MoveReq.SetReachTestIncludesAgentRadius(false);
    const FAIRequestID RequestID = PFC->RequestMove(MoveReq, NavPath);
    return RequestID.IsValid() ? EStateTreeRunStatus::Running : EStateTreeRunStatus::Failed;
}

EStateTreeRunStatus FMoveToPathPointTask::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
    auto& [AIController, Actor, StuckCheckLocation, StuckCheckTime, ConsecutiveStuckNudges] =
        Context.GetInstanceData(*this);
    if (!AIController || !Actor) return EStateTreeRunStatus::Failed;
    if (!Actor->GetPathPoints().IsEmpty()) UpdateNearestForwardPathIndex(*Actor);
    constexpr float StuckDistanceThreshold = 15.0f;
    constexpr float StuckTimeThreshold = 1.5f;
    if (FVector::DistSquared(Actor->GetActorLocation(), StuckCheckLocation) > FMath::Square(StuckDistanceThreshold))
    {
        StuckCheckLocation = Actor->GetActorLocation();
        StuckCheckTime = 0.0f;
        ConsecutiveStuckNudges = 0;
    }
    else StuckCheckTime += DeltaTime;

    if (StuckCheckTime >= StuckTimeThreshold && AIController->GetMoveStatus() == EPathFollowingStatus::Moving)
    {
        StuckCheckTime = 0.0f;
        constexpr int32 MaxStuckNudges = 3;
        if (ConsecutiveStuckNudges >= MaxStuckNudges) return EStateTreeRunStatus::Succeeded;
        const TArray<FVector>& AllPoints = Actor->GetPathPoints();
        const int32 Idx = Actor->GetCurrentPathIndex();
        if (AllPoints.IsValidIndex(Idx))
        {
            ++ConsecutiveStuckNudges;
            const FVector ToTarget = (AllPoints[Idx] - Actor->GetActorLocation()).GetSafeNormal2D();
            TArray<AActor*> OverlappingActors;
            Actor->GetCapsuleComponent()->GetOverlappingActors(OverlappingActors);
            FHitResult BlockHit;
            const float CapsuleRadius = Actor->GetCapsuleComponent()->GetScaledCapsuleRadius();
            const FVector TraceStart = Actor->GetActorLocation() + ToTarget * (CapsuleRadius + 5.0f);
            const FVector TraceEnd = TraceStart + ToTarget * 50.0f;
            FCollisionQueryParams QueryParams;
            QueryParams.AddIgnoredActor(Actor);
            Actor->LaunchCharacter(ToTarget * 100.0f + FVector::UpVector * 150.0f, true, true);
        }
    }

    TD_LOG_INFO(TEXT("FMoveToPathPointTask::Tick -> %s Speed = %.1f, MovementMode = %d, PFC status = %d"),
                *Actor->GetName(), Actor->GetCharacterMovement()->Velocity.Size(),
                static_cast<int32>(Actor->GetCharacterMovement()->MovementMode),
                static_cast<int32>(AIController->GetMoveStatus()));
    return AIController->GetMoveStatus() == EPathFollowingStatus::Idle ? EStateTreeRunStatus::Succeeded :
                                                                         EStateTreeRunStatus::Running;
}

void FMoveToPathPointTask::ExitState(FStateTreeExecutionContext& Context,
                                     const FStateTreeTransitionResult& Transition) const
{
    if (const FMoveToPathPointTaskInstanceData& Data = Context.GetInstanceData(*this); Data.AIController)
        Data.AIController->StopMovement();
}
