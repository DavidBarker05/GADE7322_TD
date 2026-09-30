#pragma once

#include "CoreMinimal.h"

#include "TowerDefencePawns/AI/TowerDefencePawnAIController.h"

#include "MageAIController.generated.h"

class AMage;
class ATowerDefencePawn;

UCLASS(Abstract)
class AMageAIController : public ATowerDefencePawnAIController
{
    GENERATED_BODY()

public:
    AMageAIController();

    virtual void Tick(float DeltaTime) override;

    virtual void SetControllerActive(bool bActive) override;

protected:
    virtual void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus) override;

public:
    UFUNCTION(BlueprintPure)
    AMage* GetMage() const;

    const TArray<ATowerDefencePawn*>& GetVisibleFriendlies() const { return VisibleFriendlies; }
    TArray<ATowerDefencePawn*>& GetVisibleFriendlies() { return VisibleFriendlies; }

    const TArray<ATowerDefencePawn*>& GetVisibleEnemies() const { return VisibleEnemies; }
    TArray<ATowerDefencePawn*>& GetVisibleEnemies() { return VisibleEnemies; }

private:
    bool IsOtherPawnFriendly(const ATowerDefencePawn* OtherPawn) const;

    bool IsThisATargetForOtherPawn(const ATowerDefencePawn* OtherPawn) const;

    UPROPERTY(EditDefaultsOnly, Category = "AI",
              meta = (AllowPrivateAccess = true, ClampMin = 0.0, UIMin = 0.0, ClampMax = 16.667, UIMax = 16.667,
                      Units = "Hertz"))
    float VisionUpdateFrequency = 5.0f;

    float TimeSinceLastVisionUpdate = 0.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI", meta = (AllowPrivateAccess = true))
    TArray<ATowerDefencePawn*> VisibleFriendlies;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI", meta = (AllowPrivateAccess = true))
    TArray<ATowerDefencePawn*> VisibleEnemies;
};
