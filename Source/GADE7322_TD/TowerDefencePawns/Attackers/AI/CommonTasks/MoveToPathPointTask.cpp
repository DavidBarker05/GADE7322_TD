#include "TowerDefencePawns/Attackers/AI/CommonTasks/MoveToPathPointTask.h"

#include "CustomLog.h"
#include "Navigation/PathFollowingComponent.h"
#include "StateTreeExecutionContext.h"

EStateTreeRunStatus FMoveToPathPointTask::EnterState(FStateTreeExecutionContext& Context,
                                                     const FStateTreeTransitionResult& Transition) const
{
    const auto& [AIController, Actor] = Context.GetInstanceData(*this);
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
    if (Actor->GetPathPoints().IsEmpty())
    {
        TD_LOG_ERROR(TEXT("FMoveToPathPointTask::EnterState -> Attacker has no path"));
        return EStateTreeRunStatus::Failed;
    }

    // Already at the current point (e.g. re-entering this state after finishing the last leg) -
    // advance before moving, so we always walk towards the NEXT point rather than re-arriving here
    if (FVector::Dist(Actor->GetActorLocation(), Actor->GetCurrentPathPoint()) <=
        Actor->GetPathTargetAcceptanceDistance() + KINDA_SMALL_NUMBER)
    {
        if (!Actor->AdvanceToNextPoint())
        {
            TD_LOG_ERROR(TEXT("FMoveToPathPointTask::EnterState -> Attacker has no more points on path to move to"));
            return EStateTreeRunStatus::Failed;
        }
    }

    switch (AIController->MoveToLocation(Actor->GetCurrentPathPoint(), Actor->GetPathTargetAcceptanceDistance()))
    {
        case EPathFollowingRequestResult::AlreadyAtGoal:
            return EStateTreeRunStatus::Succeeded;
        case EPathFollowingRequestResult::RequestSuccessful:
            return EStateTreeRunStatus::Running;
        default:
            return EStateTreeRunStatus::Failed;
    }
}

EStateTreeRunStatus FMoveToPathPointTask::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
    const auto& [AIController, Actor] = Context.GetInstanceData(*this);
    if (!AIController || !Actor) return EStateTreeRunStatus::Failed;
    return AIController->GetMoveStatus() == EPathFollowingStatus::Idle ? EStateTreeRunStatus::Succeeded :
                                                                          EStateTreeRunStatus::Running;
}

void FMoveToPathPointTask::ExitState(FStateTreeExecutionContext& Context,
                                     const FStateTreeTransitionResult& Transition) const
{
    if (const FMoveToPathPointTaskInstanceData& Data = Context.GetInstanceData(*this); Data.AIController)
        Data.AIController->StopMovement();
}
