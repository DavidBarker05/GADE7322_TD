#pragma once

#include "CoreMinimal.h"

#include "TowerDefencePawns/TowerDefencePawn.h"

template<typename T>
const ATowerDefencePawn* SelectTarget(TArray<ATowerDefencePawn*>& Array, T BaseLine,
                                      TFunction<T(const ATowerDefencePawn*)> GetCurrentVal,
                                      TFunction<bool(const T&, const T&)> Predicate)
{
    const ATowerDefencePawn* Result = nullptr;
    for (int32 i = Array.Num() - 1; i >= 0; --i)
    {
        const ATowerDefencePawn* Enemy = Array[i];
        if (!IsValid(Enemy) || !Enemy->IsPawnActive())
        {
            Array.RemoveAt(i, EAllowShrinking::No);
            continue;
        }
        T CurrentVal = GetCurrentVal(Enemy);
        if (!Result || Predicate(CurrentVal, BaseLine))
        {
            Result = Enemy;
            BaseLine = CurrentVal;
        }
    }
    return Result;
}

inline const ATowerDefencePawn* SelectClosestTarget(TArray<ATowerDefencePawn*>& Array, const ATowerDefencePawn* Pawn)
{
    const FVector PawnLoc = Pawn->GetActorLocation();

    auto GetCurrentVal = [&PawnLoc](const ATowerDefencePawn* Enemy) -> float
    { return FVector::Dist2D(PawnLoc, Enemy->GetActorLocation()) - Enemy->GetOccupiedRadius(); };

    auto Predicate = [](const float& CurrentDist, const float& ClosestDist) -> bool
    { return CurrentDist < ClosestDist; };

    return SelectTarget<float>(Array, TNumericLimits<float>::Max(), GetCurrentVal, Predicate);
}
