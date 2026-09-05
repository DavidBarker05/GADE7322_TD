#include "TowerDefencePawns/Defenders/AI/CommonTasks/MoveToStartingPointTask.h"

#include "CustomLog.h"
#include "Navigation/PathFollowingComponent.h"
#include "StateTreeExecutionContext.h"
#include "TowerDefencePawns/Defenders/DefenderSpot.h"

EStateTreeRunStatus FMoveToStartingPointTask::EnterState(FStateTreeExecutionContext& Context,
                                                         const FStateTreeTransitionResult& Transition) const
{
    auto& [AIController, Actor, AcceptanceRadius, StuckCheckLocation, StuckCheckTime, ConsecutiveStuckNudges] =
        Context.GetInstanceData(*this);
    StuckCheckTime = 0.0f;
    ConsecutiveStuckNudges = 0;
    if (Actor) StuckCheckLocation = Actor->GetActorLocation();
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
    const float DistToHome = FVector::Dist2D(Actor->GetActorLocation(), Destination);

    if (DistToHome <= AcceptanceRadius - KINDA_SMALL_NUMBER) return EStateTreeRunStatus::Succeeded;

    if (AIController->GetMoveStatus() == EPathFollowingStatus::Moving) return EStateTreeRunStatus::Running;

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
    auto& [AIController, Actor, AcceptanceRadius, StuckCheckLocation, StuckCheckTime, ConsecutiveStuckNudges] =
        Context.GetInstanceData(*this);
    if (!AIController || !Actor) return EStateTreeRunStatus::Failed;

    constexpr float StuckDistanceThreshold = 15.0f;
    constexpr float StuckTimeThreshold = 1.5f;
    if (FVector::DistSquared(Actor->GetActorLocation(), StuckCheckLocation) > FMath::Square(StuckDistanceThreshold))
    {
        StuckCheckLocation = Actor->GetActorLocation();
        StuckCheckTime = 0.0f;
        ConsecutiveStuckNudges = 0;
    }
    else StuckCheckTime += DeltaTime;

    if (StuckCheckTime >= StuckTimeThreshold && AIController->GetMoveStatus() == EPathFollowingStatus::Moving)
    {
        StuckCheckTime = 0.0f;
        constexpr int32 MaxStuckNudges = 3;
        if (ConsecutiveStuckNudges >= MaxStuckNudges) return EStateTreeRunStatus::Succeeded;
        if (Actor->GetSpawnPoint())
        {
            ++ConsecutiveStuckNudges;
            const FVector ToTarget =
                (Actor->GetSpawnPoint()->GetActorLocation() - Actor->GetActorLocation()).GetSafeNormal2D();
            Actor->LaunchCharacter(ToTarget * 100.0f + FVector::UpVector * 150.0f, true, true);
        }
    }

    return AIController->GetMoveStatus() == EPathFollowingStatus::Idle ? EStateTreeRunStatus::Succeeded :
                                                                         EStateTreeRunStatus::Running;
}

void FMoveToStartingPointTask::ExitState(FStateTreeExecutionContext& Context,
                                         const FStateTreeTransitionResult& Transition) const
{
    if (const FMoveToStartingPointTaskInstanceData& Data = Context.GetInstanceData(*this); Data.AIController)
        Data.AIController->StopMovement();
}
