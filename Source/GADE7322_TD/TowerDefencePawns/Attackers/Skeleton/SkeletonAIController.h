#pragma once

#include "CoreMinimal.h"

#include "AIController.h"

#include "SkeletonAIController.generated.h"

class ASkeletonPawn;
class ATowerDefencePawn;
struct FAIStimulus;
class UAISenseConfig_Proximity;
class UStateTreeAIComponent;

UCLASS(Abstract)
class GADE7322_TD_API ASkeletonAIController : public AAIController
{
    GENERATED_BODY()

public:
    ASkeletonAIController();

    virtual void Tick(float DeltaTime) override;

public:
    UFUNCTION(BlueprintPure)
    ASkeletonPawn* GetSkeletonPawn() const;

    void SetControllerActive(bool bActive);

private:
    UFUNCTION()
    void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

private:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI", meta = (AllowPrivateAccess = "true"))
    UAISenseConfig_Proximity* ProximityConfig;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI", meta = (AllowPrivateAccess = "true"))
    UStateTreeAIComponent* StateTree;

    UPROPERTY(EditDefaultsOnly, Category = "AI",
              meta = (AllowPrivateAccess = "true", Units = "Hertz", ClampMin = "0.0", UIMin = "0.0",
                      ClampMax = "16.667", UIMax = "16.667"))
    float VisionUpdateFrequency = 5.0f;

    TArray<ATowerDefencePawn*> VisibleEnemies;
    float TimeSinceLastVisionUpdate = 0.0f;
};
