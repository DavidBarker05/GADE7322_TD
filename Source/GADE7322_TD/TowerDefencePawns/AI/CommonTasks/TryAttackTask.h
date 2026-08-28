#pragma once

#include "CoreMinimal.h"

#include "Tasks/StateTreeAITask.h"
#include "TowerDefencePawn.h"

#include "TryAttackTask.generated.h"

USTRUCT(meta = (DisplayName = "Maintain Distance to Target Instance Data"))
struct FTryAttackTaskInstanceData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, Category = Context)
    TObjectPtr<ATowerDefencePawn> Actor = nullptr;
};

USTRUCT(meta = (DisplayName = "Try Attack", Category = "AI|CommonTask"))
struct GADE7322_TD_API FTryAttackTask : public FStateTreeAIActionTaskBase
{
    GENERATED_BODY()

    using FInstanceDataType = FTryAttackTaskInstanceData;

    virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

    virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context,
                                           const FStateTreeTransitionResult& Transition) const override;

#if WITH_EDITOR
    virtual FName GetIconName() const override { return FName("StateTreeEditorStyle|Node.Function"); }

    virtual FColor GetIconColor() const override { return UE::StateTree::Colors::Grey; }
#endif
};
