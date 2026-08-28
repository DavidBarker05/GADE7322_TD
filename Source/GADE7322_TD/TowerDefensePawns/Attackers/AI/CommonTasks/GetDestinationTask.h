#pragma once

#include "CoreMinimal.h"

#include "StateTreeTaskBase.h"

#include "GetDestinationTask.generated.h"

USTRUCT(meta = (DisplayName = "Get Destination Instance Data"))
struct FGetDestinationTaskInstanceData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, Category = Context)
    TObjectPtr<APawn> Actor = nullptr;

    UPROPERTY(EditAnywhere, Category = Output)
    FVector Destination;
};

USTRUCT(meta = (DisplayName = "Get Destination", Category = "AI|Attacker|CommonTask"))
struct GADE7322_TD_API FGetDestinationTask : public FStateTreeTaskCommonBase
{
    GENERATED_BODY()

    using FInstanceDataType = FGetDestinationTaskInstanceData;

    virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

    virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context,
                                           const FStateTreeTransitionResult& Transition) const override;

#if WITH_EDITOR
    virtual FName GetIconName() const override { return FName("StateTreeEditorStyle|Node.Find"); }

    virtual FColor GetIconColor() const override { return UE::StateTree::Colors::Grey; }
#endif
};
