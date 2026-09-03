#pragma once

#include "CoreMinimal.h"

#include "TowerDefencePawns/AI/TowerDefencePawnAIController.h"

#include "SkeletonAIController.generated.h"

class ASkeletonPawn;

UCLASS(Abstract)
class GADE7322_TD_API ASkeletonAIController : public ATowerDefencePawnAIController
{
    GENERATED_BODY()

public:
    ASkeletonAIController();

    virtual void Tick(float DeltaTime) override;

    virtual void SetControllerActive(bool bActive) override;

    UFUNCTION(BlueprintPure)
    ASkeletonPawn* GetSkeletonPawn() const;

private:
    UPROPERTY(EditDefaultsOnly, Category = "AI",
              meta = (AllowPrivateAccess = true, ClampMin = 0.0, UIMin = 0.0, ClampMax = 16.667, UIMax = 16.667,
                      Units = "Hertz"))
    float VisionUpdateFrequency = 5.0f;

    float TimeSinceLastVisionUpdate = 0.0f;
};
