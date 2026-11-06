#pragma once

#include "TDCore.h"

#include "Blueprint/UserWidget.h"

#include "PauseScreenWidget.generated.h"

class UButton;
class UCheckBox;
class UControlsWidget;
class UGuideWidget;

UCLASS(Abstract)
class GADE7322_TD_API UPauseScreenWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual bool Initialize() override;

    UFUNCTION()
    void ShowMainPanel() const;

protected:
    UFUNCTION()
    virtual void ResumeGame() const;

    UFUNCTION()
    virtual void RestartGame() const;

    UFUNCTION()
    virtual void OpenGuide() const;

    UFUNCTION()
    virtual void OpenControls() const;

    UFUNCTION()
    virtual void LoadMenu() const;

    UFUNCTION()
    virtual void QuitGame() const;

    UFUNCTION()
    virtual void HandleAutoPlayChanged(bool bIsChecked) const;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UButton* ResumeButton;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UCheckBox* AutoPlayToggle;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UButton* RestartButton;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UButton* GuideButton;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UGuideWidget* GuideWidget;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UButton* ControlsButton;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UControlsWidget* ControlsWidget;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UButton* MenuButton;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UButton* QuitButton;

    UPROPERTY(EditDefaultsOnly)
    TSoftObjectPtr<UWorld> MainMenu;
};
