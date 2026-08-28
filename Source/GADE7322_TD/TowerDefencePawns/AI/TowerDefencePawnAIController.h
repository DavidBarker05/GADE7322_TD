#pragma once

#include "CoreMinimal.h"

#include "AIController.h"

#include "TowerDefencePawnAIController.generated.h"

class ATowerDefencePawn;
struct FAIStimulus;
class UAISenseConfig_Proximity;
class UStateTreeAIComponent;

UCLASS(Abstract)
class GADE7322_TD_API ATowerDefencePawnAIController : public AAIController
{
    GENERATED_BODY()

public:
    ATowerDefencePawnAIController();

protected:
    virtual void BeginPlay() override;

public:
    virtual void SetControllerActive(bool bActive);

    const TArray<ATowerDefencePawn*>& GetVisiblePawns() const { return VisiblePawns; }
    TArray<ATowerDefencePawn*>& GetVisiblePawns() { return VisiblePawns; }

    const UAISenseConfig_Proximity* GetProximityConfig() const { return ProximityConfig; }
    UAISenseConfig_Proximity* GetProximityConfig() { return ProximityConfig; }

    const UStateTreeAIComponent* GetStateTree() const { return StateTree; }
    UStateTreeAIComponent* GetStateTree() { return StateTree; }

    const ATowerDefencePawn* GetTowerDefensePawn() const;

protected:
    UFUNCTION()
    virtual void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

private:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI", meta = (AllowPrivateAccess = true))
    TArray<ATowerDefencePawn*> VisiblePawns;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI", meta = (AllowPrivateAccess = true))
    UAISenseConfig_Proximity* ProximityConfig;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI", meta = (AllowPrivateAccess = true))
    UStateTreeAIComponent* StateTree;
};
