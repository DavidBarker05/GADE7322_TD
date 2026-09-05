#pragma once

#include "CoreMinimal.h"

#include "AIController.h"
#include "Tasks/StateTreeAITask.h"
#include "TowerDefencePawns/Defenders/Defender.h"

#include "MoveToStartingPointTask.generated.h"

USTRUCT(meta = (DisplayName = "Move To Starting Point Instance Data"))
struct FMoveToStartingPointTaskInstanceData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, Category = Context, meta = (DisplayName = "AI Controller"))
    TObjectPtr<AAIController> AIController = nullptr;

    UPROPERTY(EditAnywhere, Category = Context)
    TObjectPtr<ADefender> Actor = nullptr;

    UPROPERTY(EditAnywhere, Category = Parameter)
    float AcceptanceRadius = 10.0f;

    UPROPERTY(Transient)
    FVector StuckCheckLocation = FVector::ZeroVector;

    UPROPERTY(Transient)
    float StuckCheckTime = 0.0f;

    UPROPERTY(Transient)
    int32 ConsecutiveStuckNudges = 0;
};

USTRUCT(meta = (DisplayName = "Move To Starting Point", Category = "AI|Defender|CommonTask"))
struct GADE7322_TD_API FMoveToStartingPointTask : public FStateTreeAIActionTaskBase
{
    GENERATED_BODY()

    using FInstanceDataType = FMoveToStartingPointTaskInstanceData;

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
