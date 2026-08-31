#pragma once

#include "CoreMinimal.h"

#include "TowerDefencePawns/TowerDefencePawn.h"

template<typename T>
ATowerDefencePawn* SelectTarget(TArray<ATowerDefencePawn*>& Array, T BaseLine,
                                TFunction<T(const ATowerDefencePawn*)> GetCurrentVal,
                                TFunction<bool(const T&, const T&)> Predicate)
{
    ATowerDefencePawn* Result = nullptr;
    // Why couldn't we just make this like the STL? :(
    for (int32 i = Array.Num() - 1; i >= 0; --i)
    {
        ATowerDefencePawn* Enemy = Array[i];
        if (!IsValid(Enemy) || !Enemy->IsPawnActive())
        {
            // Faster erase, because move last element to current element and then pops
            // and since we already checked last element it's safe to just continue,
            // and we don't care about the order the elements are
            Array.RemoveAtSwap(i, EAllowShrinking::No);
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

inline ATowerDefencePawn* SelectClosestTarget(TArray<ATowerDefencePawn*>& Array, const ATowerDefencePawn* Pawn)
{
    const FVector PawnLoc = Pawn->GetActorLocation();

    auto GetCurrentVal = [&PawnLoc](const ATowerDefencePawn* Enemy) -> float
    { return FVector::Dist2D(PawnLoc, Enemy->GetActorLocation()) - Enemy->GetOccupiedRadius(); };

    auto Predicate = [](const float& CurrentDist, const float& ClosestDist) -> bool
    { return CurrentDist < ClosestDist; };

    return SelectTarget<float>(Array, TNumericLimits<float>::Max(), GetCurrentVal, Predicate);
}
