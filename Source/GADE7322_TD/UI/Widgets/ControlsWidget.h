#pragma once

#include "CoreMinimal.h"

#include "Blueprint/UserWidget.h"

#include "ControlsWidget.generated.h"

class UTextBlock;
class UButton;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBackClicked);

UCLASS(Abstract)
class GADE7322_TD_API UControlsWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual bool Initialize() override;

    UPROPERTY(BlueprintAssignable)
    FOnBackClicked OnBackClicked;

protected:
    UFUNCTION()
    virtual void ReturnToPreviousScreen() const;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UTextBlock* ControlsText;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UButton* BackButton;
};
