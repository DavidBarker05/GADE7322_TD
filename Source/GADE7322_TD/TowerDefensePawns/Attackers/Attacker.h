#pragma once

#include "CoreMinimal.h"

#include "TowerDefensePawns/TowerDefensePawn.h"

#include "Attacker.generated.h"

UCLASS(Abstract)
class GADE7322_TD_API AAttacker : public ATowerDefensePawn
{
    GENERATED_BODY()

public:
    AAttacker();

public:
    int32 GetCurrencyOnDeath() const { return CurrencyOnDeath; }

    void SetPathPoints(const TArray<FVector>& InPathPoints) { PathPoints = InPathPoints; }

    const TArray<FVector>& GetPathPoints() const { return PathPoints; }

    int32 GetCurrentPathIndex() const { return CurrentPathIndex; }

    UFUNCTION(BlueprintCallable, Category = "AI")
    FVector GetCurrentPathPoint() const
    {
        return PathPoints.IsValidIndex(CurrentPathIndex) ? PathPoints[CurrentPathIndex] : FVector::ZeroVector;
    }

    UFUNCTION(BlueprintCallable, Category = "AI")
    virtual bool AdvanceToNextPoint()
    {
        CurrentPathIndex++;
        return PathPoints.IsValidIndex(CurrentPathIndex);
    }

    float GetPathTargetAcceptanceDistance() const { return PathTargetAcceptanceDistance; }

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Currency",
              meta = (AllowPrivateAccess = "true", ClampMin = "0", ClampMax = "0"))
    int32 CurrencyOnDeath;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI", meta = (AllowPrivateAccess = "true"))
    TArray<FVector> PathPoints;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI", meta = (AllowPrivateAccess = "true"))
    int32 CurrentPathIndex = 0;

private:
    // Distance to target on path that the AI is considered at that point and will move to next destination
    // I can't think of better name :/
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI",
              meta = (AllowPrivateAccess = "true", Units = "m", ClampMin = "0.0", UIMin = "0.0"))
    float PathTargetAcceptanceDistance = 0.5f;
};
