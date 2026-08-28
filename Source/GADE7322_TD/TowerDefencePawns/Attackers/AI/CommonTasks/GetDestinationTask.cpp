#include "TowerDefencePawns/Attackers/AI/CommonTasks/GetDestinationTask.h"

#include "CustomLog.h"
#include "StateTreeExecutionContext.h"
#include "TowerDefencePawns/Attackers/Attacker.h"

EStateTreeRunStatus FGetDestinationTask::EnterState(FStateTreeExecutionContext& Context,
                                                    const FStateTreeTransitionResult& Transition) const
{
    FGetDestinationTaskInstanceData& Data = Context.GetInstanceData(*this);
    if (!Data.Actor)
    {
        TD_LOG_ERROR(TEXT("FGetDestinationTask::EnterState -> Actor context is nullptr"));
        return EStateTreeRunStatus::Failed;
    }
    if (AAttacker* Attacker = Cast<AAttacker>(Data.Actor))
    {
        if (Attacker->GetPathPoints().IsEmpty())
        {
            TD_LOG_ERROR(TEXT("FGetDestinationTask::EnterState -> Attacker has no path"));
            return EStateTreeRunStatus::Failed;
        }
        if (FVector::Dist(Attacker->GetActorLocation(), Attacker->GetCurrentPathPoint()) <=
            Attacker->GetPathTargetAcceptanceDistance() + KINDA_SMALL_NUMBER)
        {
            if (!Attacker->AdvanceToNextPoint())
            {
                TD_LOG_ERROR(TEXT("FGetDestinationTask::EnterState -> Attacker has no more points on path to move to"));
                return EStateTreeRunStatus::Failed;
            }
        }
        Data.Destination = Attacker->GetCurrentPathPoint();
        return EStateTreeRunStatus::Succeeded;
    }
    TD_LOG_ERROR(TEXT("FGetDestinationTask::EnterState -> Actor context must be type AAttacker"));
    return EStateTreeRunStatus::Failed;
}
