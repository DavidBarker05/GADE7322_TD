#include "TowerDefencePawns/Defenders/AI/CommonTasks/GetStartingPointTask.h"

#include "CustomLog.h"
#include "StateTreeExecutionContext.h"

EStateTreeRunStatus FGetStartingPointTask::EnterState(FStateTreeExecutionContext& Context,
                                                      const FStateTreeTransitionResult& Transition) const
{
    auto& [Actor, StartingPoint] = Context.GetInstanceData(*this);
    if (!Actor)
    {
        TD_LOG_ERROR(TEXT("FGetStartingPointTask::EnterState -> Actor context is nullptr"));
        return EStateTreeRunStatus::Failed;
    }
    if (!Actor->GetSpawnPoint())
    {
        TD_LOG_ERROR(TEXT("FGetStartingPointTask::EnterState -> Actor context doesn't have spawn point"));
        return EStateTreeRunStatus::Failed;
    }
    StartingPoint = Actor->GetSpawnPoint()->GetActorLocation();
    return EStateTreeRunStatus::Succeeded;
}
