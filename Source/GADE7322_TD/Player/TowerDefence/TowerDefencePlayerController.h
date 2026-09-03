#pragma once

#include "CoreMinimal.h"

#include "GameFramework/PlayerController.h"

#include "TowerDefencePlayerController.generated.h"

class UInputMappingContext;
class UPauseScreenWidget;
class UPlayerHUDWidget;

UCLASS()
class GADE7322_TD_API ATowerDefencePlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    void TogglePause();

protected:
    virtual void SetupInputComponent() override;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = true))
    TArray<UInputMappingContext*> DefaultMappingContexts;
};
