#include "TowerDefencePawns/Attackers/AI/CommonTasks/MoveToPathPointTask.h"

#include "CustomLog.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "StateTreeExecutionContext.h"

namespace
{
    // Steer at a point this many ahead of the one currently being walked to. The points are close together, so aiming
    // slightly ahead rounds off the tiny corners between them instead of stop-start turning at every one
    constexpr int32 LookAheadPoints = 2;

    // Closest point at or after the attacker's current index (never goes backwards along the path)
    int32 UpdateNearestForwardPathIndex(AAttacker& Actor)
    {
        const TArray<FVector>& AllPoints = Actor.GetPathPoints();
        const int32 SearchStart = Actor.GetCurrentPathIndex();
        int32 NearestIndex = SearchStart;
        float NearestDistSq = TNumericLimits<float>::Max();
        for (int32 i = SearchStart; i < AllPoints.Num(); ++i)
        {
            const float DistSq = FVector::DistSquared2D(Actor.GetActorLocation(), AllPoints[i]);
            if (DistSq < NearestDistSq)
            {
                NearestDistSq = DistSq;
                NearestIndex = i;
            }
        }
        Actor.SetCurrentPathIndex(NearestIndex);
        return NearestIndex;
    }

    // Moves the current path index past every point the attacker has reached or already walked past. Returns true once
    // the final point of the path has been reached
    bool AdvancePathIndex(AAttacker& Actor)
    {
        const TArray<FVector>& Points = Actor.GetPathPoints();
        const int32 LastIndex = Points.Num() - 1;
        const FVector Location = Actor.GetActorLocation();
        const float Acceptance = Actor.GetPathTargetAcceptanceDistance();
        int32 Index = Actor.GetCurrentPathIndex();
        while (Index < LastIndex)
        {
            const bool bReached = FVector::Dist2D(Location, Points[Index]) <= Acceptance;
            // Also counts as done with a point once we're beyond it along the path's direction, so a frame hitch (or
            // being a bit off to the side) can't leave the attacker circling a point it never got close enough to
            const bool bPassed = FVector2D::DotProduct(FVector2D(Location - Points[Index]),
                                                       FVector2D(Points[Index + 1] - Points[Index])) > 0.0f;
            if (!bReached && !bPassed) break;
            ++Index;
        }
        Actor.SetCurrentPathIndex(Index);
        return Index >= LastIndex && FVector::Dist2D(Location, Points[LastIndex]) <= Acceptance;
    }

    // Safety net for physically getting stuck on something: jump a few times, then skip ahead along the path
    void HandleStuck(FMoveToPathPointTaskInstanceData& Data, AAttacker& Actor, const float DeltaTime)
    {
        constexpr float StuckDistanceThreshold = 15.0f;
        constexpr float StuckTimeThreshold = 1.5f;
        constexpr int32 MaxStuckNudges = 3;

        if (FVector::DistSquared(Actor.GetActorLocation(), Data.StuckCheckLocation) >
            FMath::Square(StuckDistanceThreshold))
        {
            Data.StuckCheckLocation = Actor.GetActorLocation();
            Data.StuckCheckTime = 0.0f;
            Data.ConsecutiveStuckNudges = 0;
            return;
        }
        Data.StuckCheckTime += DeltaTime;
        if (Data.StuckCheckTime < StuckTimeThreshold) return;
        Data.StuckCheckTime = 0.0f;

        const TArray<FVector>& Points = Actor.GetPathPoints();
        const int32 Index = Actor.GetCurrentPathIndex();
        if (!Points.IsValidIndex(Index)) return;

        if (Data.ConsecutiveStuckNudges >= MaxStuckNudges)
        {
            const int32 NewIndex = FMath::Min(Index + FMath::RandRange(1, 10), Points.Num() - 1);
            Actor.SetActorLocation(Points[NewIndex], false, nullptr, ETeleportType::TeleportPhysics);
            Actor.SetCurrentPathIndex(NewIndex);
            if (UCharacterMovementComponent* Movement = Actor.GetCharacterMovement())
                Movement->StopMovementImmediately();
            Data.StuckCheckLocation = Actor.GetActorLocation();
            Data.ConsecutiveStuckNudges = 0;
            if (Data.bRejoining) // Back on the path now, so no need for the AI to keep trying to get there
            {
                Data.AIController->StopMovement();
                Data.bRejoining = false;
            }
            return;
        }
        ++Data.ConsecutiveStuckNudges;
        const FVector ToTarget = (Points[Index] - Actor.GetActorLocation()).GetSafeNormal2D();
        Actor.LaunchCharacter(ToTarget * 100.0f + FVector::UpVector * 150.0f, true, true);
    }
} // namespace

EStateTreeRunStatus FMoveToPathPointTask::EnterState(FStateTreeExecutionContext& Context,
                                                     const FStateTreeTransitionResult& Transition) const
{
    FMoveToPathPointTaskInstanceData& Data = Context.GetInstanceData(*this);
    Data.StuckCheckTime = 0.0f;
    Data.ConsecutiveStuckNudges = 0;
    Data.bRejoining = false;
    if (Data.Actor) Data.StuckCheckLocation = Data.Actor->GetActorLocation();
    if (!Data.AIController)
    {
        TD_LOG_ERROR(TEXT("FMoveToPathPointTask::EnterState -> AI Controller context is nullptr"));
        return EStateTreeRunStatus::Failed;
    }
    if (!Data.Actor)
    {
        TD_LOG_ERROR(TEXT("FMoveToPathPointTask::EnterState -> Actor context is nullptr"));
        return EStateTreeRunStatus::Failed;
    }
    AAttacker& Actor = *Data.Actor;
    const TArray<FVector>& AllPoints = Actor.GetPathPoints();
    if (AllPoints.IsEmpty())
    {
        TD_LOG_ERROR(TEXT("FMoveToPathPointTask::EnterState -> Attacker has no path"));
        return EStateTreeRunStatus::Failed;
    }

    const int32 StartIndex = UpdateNearestForwardPathIndex(Actor);
    if (AllPoints.Num() - StartIndex < 2) return EStateTreeRunStatus::Succeeded;

    const FVector& RejoinPoint = AllPoints[StartIndex];
    if (FVector::Dist2D(Actor.GetActorLocation(), RejoinPoint) > Actor.GetPathRejoinDistance())
    {
        // Stops once within half the rejoin distance, which is well inside what manual steering can handle
        const float AcceptanceRadius = Actor.GetPathRejoinDistance() * 0.5f;
        if (Data.AIController->MoveToLocation(RejoinPoint, AcceptanceRadius, true, true, true, false) ==
            EPathFollowingRequestResult::RequestSuccessful)
            Data.bRejoining = true;
    }
    return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus FMoveToPathPointTask::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
    FMoveToPathPointTaskInstanceData& Data = Context.GetInstanceData(*this);
    if (!Data.AIController || !Data.Actor) return EStateTreeRunStatus::Failed;
    AAttacker& Actor = *Data.Actor;
    const TArray<FVector>& Points = Actor.GetPathPoints();
    if (Points.IsEmpty()) return EStateTreeRunStatus::Failed;

    if (Data.bRejoining)
    {
        const FVector& RejoinPoint = Points[FMath::Clamp(Actor.GetCurrentPathIndex(), 0, Points.Num() - 1)];
        const float RejoinRadius = Actor.GetPathRejoinDistance() * 0.5f;
        if (Data.AIController->GetMoveStatus() == EPathFollowingStatus::Idle ||
            FVector::Dist2D(Actor.GetActorLocation(), RejoinPoint) <= RejoinRadius)
        {
            Data.AIController->StopMovement();
            Data.bRejoining = false; // Manual steering takes over from here, in this same tick
        }
    }

    if (!Data.bRejoining)
    {
        if (AdvancePathIndex(Actor)) return EStateTreeRunStatus::Succeeded;

        const int32 AimIndex = FMath::Min(Actor.GetCurrentPathIndex() + LookAheadPoints, Points.Num() - 1);
        const FVector ToAim = Points[AimIndex] - Actor.GetActorLocation();
        const UCharacterMovementComponent* Movement = Actor.GetCharacterMovement();
        const FVector Direction = (Movement && Movement->IsFlying()) ? ToAim.GetSafeNormal() : ToAim.GetSafeNormal2D();
        Actor.AddMovementInput(Direction, 1.0f);
    }

    HandleStuck(Data, Actor, DeltaTime);
    return EStateTreeRunStatus::Running;
}

void FMoveToPathPointTask::ExitState(FStateTreeExecutionContext& Context,
                                     const FStateTreeTransitionResult& Transition) const
{
    if (const FMoveToPathPointTaskInstanceData& Data = Context.GetInstanceData(*this); Data.AIController)
        Data.AIController->StopMovement();
}
