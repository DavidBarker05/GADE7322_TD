#include "TowerDefencePawns/AI/CommonTasks/TryAttackTask.h"

#include "CustomLog.h"
#include "StateTreeExecutionContext.h"

EStateTreeRunStatus FTryAttackTask::EnterState(FStateTreeExecutionContext& Context,
                                               const FStateTreeTransitionResult& Transition) const
{
    FTryAttackTaskInstanceData& Data = Context.GetInstanceData(*this);
    if (!Data.Actor)
    {
        TD_LOG_ERROR(TEXT("FTryAttackTask::EnterState -> Actor context is nullptr"));
        return EStateTreeRunStatus::Failed;
    }
    if (!Data.Actor->CanAttack()) return EStateTreeRunStatus::Failed;
    Data.Actor->StartAttack();
    return EStateTreeRunStatus::Succeeded;
}
