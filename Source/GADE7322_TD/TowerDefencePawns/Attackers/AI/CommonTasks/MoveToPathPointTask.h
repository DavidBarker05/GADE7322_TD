#pragma once

#include "CoreMinimal.h"

#include "AIController.h"
#include "Tasks/StateTreeAITask.h"
#include "TowerDefencePawns/Attackers/Attacker.h"

#include "MoveToPathPointTask.generated.h"

USTRUCT(meta = (DisplayName = "Move To Path Point Instance Data"))
struct FMoveToPathPointTaskInstanceData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, Category = Context, meta = (DisplayName = "AI Controller"))
    TObjectPtr<AAIController> AIController = nullptr;

    UPROPERTY(EditAnywhere, Category = Context)
    TObjectPtr<AAttacker> Actor = nullptr;
};

// Moves Actor towards its current path point, advancing to the next one first if already there.
// Acceptance radius comes from the Attacker itself (PathTargetAcceptanceDistance), not a task parameter
USTRUCT(meta = (DisplayName = "Move To Path Point", Category = "AI|Attacker|CommonTask"))
struct GADE7322_TD_API FMoveToPathPointTask : public FStateTreeAIActionTaskBase
{
    GENERATED_BODY()

    using FInstanceDataType = FMoveToPathPointTaskInstanceData;

    virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

    virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context,
                                           const FStateTreeTransitionResult& Transition) const override;

    virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, float DeltaTime) const override;

    virtual void ExitState(FStateTreeExecutionContext& Context,
                           const FStateTreeTransitionResult& Transition) const override;

#if WITH_EDITOR
    virtual FName GetIconName() const override { return FName("StateTreeEditorStyle|Node.Movement"); }

    virtual FColor GetIconColor() const override { return UE::StateTree::Colors::Grey; }
#endif
};
