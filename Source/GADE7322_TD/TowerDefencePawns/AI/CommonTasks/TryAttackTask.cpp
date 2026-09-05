#include "TowerDefencePawns/AI/CommonTasks/TryAttackTask.h"

#include "CustomLog.h"
#include "StateTreeExecutionContext.h"

EStateTreeRunStatus FTryAttackTask::EnterState(FStateTreeExecutionContext& Context,
                                               const FStateTreeTransitionResult& Transition) const
{
    auto& [Actor] = Context.GetInstanceData(*this);
    if (!Actor)
    {
        TD_LOG_ERROR(TEXT("FTryAttackTask::EnterState -> Actor context is nullptr"));
        return EStateTreeRunStatus::Failed;
    }
    if (!Actor->CanAttack()) return EStateTreeRunStatus::Failed;
    Actor->StartAttack();
    return EStateTreeRunStatus::Succeeded;
}
