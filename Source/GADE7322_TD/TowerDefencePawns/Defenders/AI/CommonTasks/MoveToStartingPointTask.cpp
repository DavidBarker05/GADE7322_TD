#include "TowerDefencePawns/Defenders/AI/CommonTasks/MoveToStartingPointTask.h"

#include "CustomLog.h"
#include "Navigation/PathFollowingComponent.h"
#include "StateTreeExecutionContext.h"
#include "TowerDefencePawns/Defenders/DefenderSpot.h"

EStateTreeRunStatus FMoveToStartingPointTask::EnterState(FStateTreeExecutionContext& Context,
                                                         const FStateTreeTransitionResult& Transition) const
{
    const auto& [AIController, Actor, AcceptanceRadius] = Context.GetInstanceData(*this);
    if (!AIController)
    {
        TD_LOG_ERROR(TEXT("FMoveToStartingPointTask::EnterState -> AI Controller context is nullptr"));
        return EStateTreeRunStatus::Failed;
    }
    if (!Actor)
    {
        TD_LOG_ERROR(TEXT("FMoveToStartingPointTask::EnterState -> Actor context is nullptr"));
        return EStateTreeRunStatus::Failed;
    }
    if (!Actor->GetSpawnPoint())
    {
        TD_LOG_ERROR(TEXT("FMoveToStartingPointTask::EnterState -> Actor context doesn't have a spawn point"));
        return EStateTreeRunStatus::Failed;
    }

    const FVector Destination = Actor->GetSpawnPoint()->GetActorLocation();

    // Rather be slightly closer, and it also skips issuing a move entirely on the common "already home" case
    if (FVector::Dist2D(Actor->GetActorLocation(), Destination) <= AcceptanceRadius - KINDA_SMALL_NUMBER)
        return EStateTreeRunStatus::Succeeded;

    switch (AIController->MoveToLocation(Destination, AcceptanceRadius))
    {
        case EPathFollowingRequestResult::AlreadyAtGoal:
            return EStateTreeRunStatus::Succeeded;
        case EPathFollowingRequestResult::RequestSuccessful:
            return EStateTreeRunStatus::Running;
        default:
            return EStateTreeRunStatus::Failed;
    }
}

EStateTreeRunStatus FMoveToStartingPointTask::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
    const auto& [AIController, Actor, AcceptanceRadius] = Context.GetInstanceData(*this);
    if (!AIController || !Actor) return EStateTreeRunStatus::Failed;
    return AIController->GetMoveStatus() == EPathFollowingStatus::Idle ? EStateTreeRunStatus::Succeeded :
                                                                          EStateTreeRunStatus::Running;
}

void FMoveToStartingPointTask::ExitState(FStateTreeExecutionContext& Context,
                                         const FStateTreeTransitionResult& Transition) const
{
    if (const FMoveToStartingPointTaskInstanceData& Data = Context.GetInstanceData(*this); Data.AIController)
        Data.AIController->StopMovement();
}
