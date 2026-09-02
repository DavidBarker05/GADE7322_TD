#pragma once

#include "CoreMinimal.h"

#include "Blueprint/UserWidget.h"

#include "PauseScreenWidget.generated.h"

class UButton;
class UCheckBox;
class UControlsWidget;

UCLASS()
class GADE7322_TD_API UPauseScreenWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    void ShowMainPanel() const;

    virtual bool Initialize() override;

protected:
    UFUNCTION()
    virtual void ResumeGame() const;

    UFUNCTION()
    virtual void RestartGame() const;

    UFUNCTION()
    virtual void OpenControls() const;

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
    UButton* ControlsButton;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UControlsWidget* ControlsWidget;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UButton* QuitButton;
};
