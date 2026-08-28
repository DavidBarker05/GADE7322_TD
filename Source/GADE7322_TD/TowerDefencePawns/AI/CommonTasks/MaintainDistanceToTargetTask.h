#pragma once

#include "CoreMinimal.h"

#include "AIController.h"
#include "Tasks/StateTreeAITask.h"

#include "MaintainDistanceToTargetTask.generated.h"

USTRUCT(meta = (DisplayName = "Maintain Distance to Target Instance Data"))
struct FMaintainDistanceToTargetTaskInstanceData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, Category = Context, meta = (DisplayName = "AI Controller"))
    TObjectPtr<AAIController> AIController = nullptr;

    UPROPERTY(EditAnywhere, Category = Context)
    TObjectPtr<APawn> Actor = nullptr;

    UPROPERTY(EditAnywhere, Category = Parameter)
    TObjectPtr<AActor> Target = nullptr;

    UPROPERTY(EditAnywhere, Category = Parameter)
    float OccupiedRadius;

    UPROPERTY(EditAnywhere, Category = Parameter)
    float TargetOccupiedRadius;

    UPROPERTY(EditAnywhere, Category = Parameter)
    float RadiusToKeepTargetWithin;
};

USTRUCT(meta = (DisplayName = "Maintain Distance to Target", Category = "AI|CommonTask"))
struct GADE7322_TD_API FMaintainDistanceToTargetTask : public FStateTreeAIActionTaskBase
{
    GENERATED_BODY()

    using FInstanceDataType = FMaintainDistanceToTargetTaskInstanceData;

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
