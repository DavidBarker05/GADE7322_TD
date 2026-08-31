#pragma once

#include "CoreMinimal.h"

#include "TowerDefencePawns/AI/TowerDefencePawnAIController.h"

#include "PlayerTowerAIController.generated.h"

class APlayerTower;

UCLASS(Abstract)
class GADE7322_TD_API APlayerTowerAIController : public ATowerDefencePawnAIController
{
    GENERATED_BODY()

public:
    APlayerTowerAIController();

    virtual void Tick(float DeltaTime) override;

    virtual void SetControllerActive(bool bActive) override;

    UFUNCTION(BlueprintPure)
    APlayerTower* GetPlayerTower() const;

private:
    UPROPERTY(EditDefaultsOnly, Category = "AI",
              meta = (AllowPrivateAccess = true, ClampMin = 0.0, UIMin = 0.0, ClampMax = 16.667, UIMax = 16.667,
                      Units = "Hertz"))
    float VisionUpdateFrequency = 5.0f;

    float TimeSinceLastVisionUpdate = 0.0f;
};
