#include "TowerDefencePawns/Attackers/AI/CommonTasks/GetDestinationTask.h"

#include "CustomLog.h"
#include "StateTreeExecutionContext.h"

EStateTreeRunStatus FGetDestinationTask::EnterState(FStateTreeExecutionContext& Context,
                                                    const FStateTreeTransitionResult& Transition) const
{
    auto& [Actor, Destination] = Context.GetInstanceData(*this);
    if (!Actor)
    {
        TD_LOG_ERROR(TEXT("FGetDestinationTask::EnterState -> Actor context is nullptr"));
        return EStateTreeRunStatus::Failed;
    }
    if (Actor->GetPathPoints().IsEmpty())
    {
        TD_LOG_ERROR(TEXT("FGetDestinationTask::EnterState -> Attacker has no path"));
        return EStateTreeRunStatus::Failed;
    }
    if (FVector::Dist(Actor->GetActorLocation(), Actor->GetCurrentPathPoint()) <=
        Actor->GetPathTargetAcceptanceDistance() + KINDA_SMALL_NUMBER)
    {
        if (!Actor->AdvanceToNextPoint())
        {
            TD_LOG_ERROR(TEXT("FGetDestinationTask::EnterState -> Attacker has no more points on path to move to"));
            return EStateTreeRunStatus::Failed;
        }
    }
    Destination = Actor->GetCurrentPathPoint();
    return EStateTreeRunStatus::Succeeded;
}
