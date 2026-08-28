#pragma once

#include "CoreMinimal.h"

#include "GameFramework/PlayerController.h"

#include "TowerDefencePlayerController.generated.h"

class UInputMappingContext;

UCLASS()
class GADE7322_TD_API ATowerDefencePlayerController : public APlayerController
{
    GENERATED_BODY()

protected:
    virtual void SetupInputComponent() override;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
    TArray<UInputMappingContext*> DefaultMappingContexts;
};
