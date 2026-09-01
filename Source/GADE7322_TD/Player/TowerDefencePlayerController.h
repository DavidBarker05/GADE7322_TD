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

protected:
    virtual void BeginPlay() override;

public:
    void TogglePause();

    bool IsGamePaused() const { return bIsPaused; }

protected:
    virtual void SetupInputComponent() override;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = true))
    TArray<UInputMappingContext*> DefaultMappingContexts;

private:
    UPROPERTY(EditDefaultsOnly, Category = "UI", meta = (AllowPrivateAccess = true))
    TSubclassOf<UPauseScreenWidget> PauseScreenWidgetClass;

    UPROPERTY(BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = true))
    UPauseScreenWidget* PauseScreenWidgetInstance;

    UPROPERTY(EditDefaultsOnly, Category = "UI", meta = (AllowPrivateAccess = true))
    TSubclassOf<UPlayerHUDWidget> PlayerHUDWidgetClass;

    UPROPERTY(BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = true))
    UPlayerHUDWidget* PlayerHUDWidgetInstance;

    UPROPERTY(BlueprintReadOnly, Category = "Game", meta = (AllowPrivateAccess = true))
    bool bIsPaused = false;
};
