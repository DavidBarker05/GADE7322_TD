#pragma once

#include "CoreMinimal.h"

#include "StateTreeTaskBase.h"
#include "TowerDefencePawns/Defenders/Defender.h"

#include "GetStartingPointTask.generated.h"

USTRUCT(meta = (DisplayName = "Get Starting Point Instance Data"))
struct FGetStartingPointTaskInstanceData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, Category = Context)
    TObjectPtr<ADefender> Actor = nullptr;

    UPROPERTY(EditAnywhere, Category = Output)
    FVector StartingPoint;
};

USTRUCT(meta = (DisplayName = "Get Starting Point", Category = "AI|Defender|CommonTask"))
struct GADE7322_TD_API FGetStartingPointTask : public FStateTreeTaskCommonBase
{
    GENERATED_BODY()

    using FInstanceDataType = FGetStartingPointTaskInstanceData;

    virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

    virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context,
                                           const FStateTreeTransitionResult& Transition) const override;

#if WITH_EDITOR
    virtual FName GetIconName() const override { return FName("StateTreeEditorStyle|Node.Find"); }

    virtual FColor GetIconColor() const override { return UE::StateTree::Colors::Grey; }
#endif
};
