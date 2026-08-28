#include "TowerDefencePawns/Attackers/AI/CommonTasks/GetDestinationTask.h"

#include "CustomLog.h"
#include "StateTreeExecutionContext.h"

EStateTreeRunStatus FGetDestinationTask::EnterState(FStateTreeExecutionContext& Context,
                                                    const FStateTreeTransitionResult& Transition) const
{
    FGetDestinationTaskInstanceData& Data = Context.GetInstanceData(*this);
    if (!Data.Actor)
    {
        TD_LOG_ERROR(TEXT("FGetDestinationTask::EnterState -> Actor context is nullptr"));
        return EStateTreeRunStatus::Failed;
    }
    if (Data.Actor->GetPathPoints().IsEmpty())
    {
        TD_LOG_ERROR(TEXT("FGetDestinationTask::EnterState -> Attacker has no path"));
        return EStateTreeRunStatus::Failed;
    }
    if (FVector::Dist(Data.Actor->GetActorLocation(), Data.Actor->GetCurrentPathPoint()) <=
        Data.Actor->GetPathTargetAcceptanceDistance() + KINDA_SMALL_NUMBER)
    {
        if (!Data.Actor->AdvanceToNextPoint())
        {
            TD_LOG_ERROR(TEXT("FGetDestinationTask::EnterState -> Attacker has no more points on path to move to"));
            return EStateTreeRunStatus::Failed;
        }
    }
    Data.Destination = Data.Actor->GetCurrentPathPoint();
    return EStateTreeRunStatus::Succeeded;
}
