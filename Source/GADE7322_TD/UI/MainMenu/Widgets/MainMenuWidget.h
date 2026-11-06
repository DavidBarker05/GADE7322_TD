#pragma once

#include "TDCore.h"

#include "Blueprint/UserWidget.h"

#include "MainMenuWidget.generated.h"

class UButton;
class UControlsWidget;
class UCreditsWidget;
class UGuideWidget;
class UTextBlock;

UCLASS(Abstract)
class UMainMenuWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual bool Initialize() override;

    UFUNCTION()
    void ShowMainPanel() const;

protected:
    UFUNCTION()
    virtual void LoadGame() const;

    UFUNCTION()
    virtual void OpenGuide() const;

    UFUNCTION()
    virtual void OpenControls() const;

    UFUNCTION()
    virtual void OpenCredits() const;

    UFUNCTION()
    virtual void QuitGame() const;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UTextBlock* Title;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UButton* PlayButton;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UButton* GuideButton;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UGuideWidget* GuideWidget;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UButton* ControlsButton;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UControlsWidget* ControlsWidget;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UButton* CreditsButton;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UCreditsWidget* CreditsWidget;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UButton* QuitButton;

    UPROPERTY(EditDefaultsOnly)
    TSoftObjectPtr<UWorld> MainLevel;
};
