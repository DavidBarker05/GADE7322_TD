#pragma once

#include "TDCore.h"

#include "GameFramework/PlayerController.h"

#include "MainMenuPlayerController.generated.h"

class UInputMappingContext;

UCLASS()
class GADE7322_TD_API AMainMenuPlayerController : public APlayerController
{
    GENERATED_BODY()
protected:
    virtual void SetupInputComponent() override;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = true))
    TArray<UInputMappingContext*> DefaultMappingContexts;
};
