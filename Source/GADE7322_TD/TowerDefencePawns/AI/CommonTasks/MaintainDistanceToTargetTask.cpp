#include "TowerDefencePawns/AI/CommonTasks/MaintainDistanceToTargetTask.h"

#include "CustomLog.h"
#include "Navigation/PathFollowingComponent.h"
#include "StateTreeExecutionContext.h"

EStateTreeRunStatus FMaintainDistanceToTargetTask::EnterState(FStateTreeExecutionContext& Context,
                                                              const FStateTreeTransitionResult& Transition) const
{
    const auto& [AIController, Actor, Target, OccupiedRadius, TargetOccupiedRadius, RadiusToKeepTargetWithin] =
        Context.GetInstanceData(*this);
    if (!AIController)
    {
        TD_LOG_ERROR(TEXT("FMaintainDistanceToTargetTask::EnterState -> AI Controller context is nullptr"));
        return EStateTreeRunStatus::Failed;
    }
    if (!Actor)
    {
        TD_LOG_ERROR(TEXT("FMaintainDistanceToTargetTask::EnterState -> Actor context is nullptr"));
        return EStateTreeRunStatus::Failed;
    }
    if (!IsValid(Target))
    {
        TD_LOG_INFO(TEXT("FMaintainDistanceToTargetTask::EnterState -> %s has no valid Target"), *Actor->GetName());
        return EStateTreeRunStatus::Failed;
    }
    const float CurrentEdgeDist =
        FVector::Dist2D(Actor->GetActorLocation(), Target->GetActorLocation()) - TargetOccupiedRadius;
    constexpr float RangeTolerance = 10.0f;
    if (CurrentEdgeDist < RadiusToKeepTargetWithin + RangeTolerance) return EStateTreeRunStatus::Succeeded;

    if (AIController->GetMoveStatus() == EPathFollowingStatus::Moving) return EStateTreeRunStatus::Running;
    const float StopEdgeDistance = (OccupiedRadius + RadiusToKeepTargetWithin) / 2.0f;
    // ReSharper disable once CppTooWideScopeInitStatement
    const float AcceptanceRadius = TargetOccupiedRadius + StopEdgeDistance;
    switch (AIController->MoveToActor(Target, AcceptanceRadius, false, false))
    {
        case EPathFollowingRequestResult::AlreadyAtGoal:
            return EStateTreeRunStatus::Succeeded;
        case EPathFollowingRequestResult::RequestSuccessful:
            return EStateTreeRunStatus::Running;
        default:
            return EStateTreeRunStatus::Failed;
    }
}

EStateTreeRunStatus FMaintainDistanceToTargetTask::Tick(FStateTreeExecutionContext& Context,
                                                        const float DeltaTime) const
{
    const auto& [AIController, Actor, Target, OccupiedRadius, TargetOccupiedRadius, RadiusToKeepTargetWithin] =
        Context.GetInstanceData(*this);
    if (!AIController || !IsValid(Target)) return EStateTreeRunStatus::Failed;
    return AIController->GetMoveStatus() == EPathFollowingStatus::Idle ? EStateTreeRunStatus::Succeeded :
                                                                         EStateTreeRunStatus::Running;
}

void FMaintainDistanceToTargetTask::ExitState(FStateTreeExecutionContext& Context,
                                              const FStateTreeTransitionResult& Transition) const
{
    if (const FMaintainDistanceToTargetTaskInstanceData& Data = Context.GetInstanceData(*this);
        Data.AIController && !IsValid(Data.Target))
        Data.AIController->StopMovement();
}
