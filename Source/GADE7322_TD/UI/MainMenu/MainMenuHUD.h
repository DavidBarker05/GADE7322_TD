#pragma once

#include "TDCore.h"

#include "GameFramework/HUD.h"

#include "MainMenuHUD.generated.h"

class UMainMenuWidget;

UCLASS(Abstract)
class GADE7322_TD_API AMainMenuHUD : public AHUD
{
    GENERATED_BODY()

protected:
    virtual void BeginPlay() override;

public:
    UFUNCTION(BlueprintCallable, Category = "UI")
    void ShowMainMenu() const;

    UFUNCTION(BlueprintCallable, Category = "UI")
    void ShowLoadingScreen() const;

private:
    UPROPERTY(EditDefaultsOnly, Category = "UI", meta = (AllowPrivateAccess = true))
    TSubclassOf<UMainMenuWidget> MainMenuWidgetClass;

    UPROPERTY(BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = true))
    UMainMenuWidget* MainMenuWidgetInstance;

    UPROPERTY(EditDefaultsOnly, Category = "UI", meta = (AllowPrivateAccess = true))
    TSubclassOf<UUserWidget> LoadingScreenWidgetClass;

    UPROPERTY(BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = true))
    UUserWidget* LoadingScreenWidgetInstance;
};
