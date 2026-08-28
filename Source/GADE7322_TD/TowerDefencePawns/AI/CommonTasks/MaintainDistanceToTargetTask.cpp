#include "TowerDefencePawns/AI/CommonTasks/MaintainDistanceToTargetTask.h"

#include "CustomLog.h"
#include "Navigation/PathFollowingComponent.h"
#include "StateTreeExecutionContext.h"

EStateTreeRunStatus FMaintainDistanceToTargetTask::EnterState(FStateTreeExecutionContext& Context,
                                                              const FStateTreeTransitionResult& Transition) const
{
    FMaintainDistanceToTargetTaskInstanceData& Data = Context.GetInstanceData(*this);
    if (!Data.AIController)
    {
        TD_LOG_ERROR(TEXT("FMaintainDistanceToTargetTask::EnterState -> AI Controller context is nullptr"));
        return EStateTreeRunStatus::Failed;
    }
    if (!Data.Actor)
    {
        TD_LOG_ERROR(TEXT("FMaintainDistanceToTargetTask::EnterState -> Actor context is nullptr"));
        return EStateTreeRunStatus::Failed;
    }
    if (!IsValid(Data.Target)) return EStateTreeRunStatus::Failed;
    if (FVector::Dist2D(Data.Actor->GetActorLocation(), Data.Target->GetActorLocation()) - Data.TargetOccupiedRadius <
        Data.RadiusToKeepTargetWithin - KINDA_SMALL_NUMBER) // Rather be slightly closer
        return EStateTreeRunStatus::Succeeded;
    const float StopEdgeDistance = (Data.OccupiedRadius + Data.RadiusToKeepTargetWithin) / 2.0f;
    const float AcceptanceRadius = Data.TargetOccupiedRadius + StopEdgeDistance;
    switch (Data.AIController->MoveToActor(Data.Target, AcceptanceRadius, false))
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
    FMaintainDistanceToTargetTaskInstanceData& Data = Context.GetInstanceData(*this);
    if (!Data.AIController || !IsValid(Data.Target)) return EStateTreeRunStatus::Failed;
    return Data.AIController->GetMoveStatus() == EPathFollowingStatus::Idle ? EStateTreeRunStatus::Succeeded :
                                                                              EStateTreeRunStatus::Running;
}

void FMaintainDistanceToTargetTask::ExitState(FStateTreeExecutionContext& Context,
                                              const FStateTreeTransitionResult& Transition) const
{
    FMaintainDistanceToTargetTaskInstanceData& Data = Context.GetInstanceData(*this);
    if (Data.AIController) Data.AIController->StopMovement();
}
