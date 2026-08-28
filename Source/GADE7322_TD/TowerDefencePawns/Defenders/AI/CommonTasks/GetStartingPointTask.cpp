#include "TowerDefencePawns/Defenders/AI/CommonTasks/GetStartingPointTask.h"

#include "Attacker.h"
#include "CustomLog.h"
#include "StateTreeExecutionContext.h"

EStateTreeRunStatus FGetStartingPointTask::EnterState(FStateTreeExecutionContext& Context,
                                                      const FStateTreeTransitionResult& Transition) const
{
    FGetStartingPointTaskInstanceData& Data = Context.GetInstanceData(*this);
    if (!Data.Actor)
    {
        TD_LOG_ERROR(TEXT("FGetStartingPointTask::EnterState -> Actor context is nullptr"));
        return EStateTreeRunStatus::Failed;
    }
    if (!Data.Actor->GetSpawnPoint())
    {
        TD_LOG_ERROR(TEXT("FGetStartingPointTask::EnterState -> Actor context doesn't have spawn point"));
        return EStateTreeRunStatus::Failed;
    }
    Data.StartingPoint = Data.Actor->GetSpawnPoint()->GetActorLocation();
    return EStateTreeRunStatus::Succeeded;
}
