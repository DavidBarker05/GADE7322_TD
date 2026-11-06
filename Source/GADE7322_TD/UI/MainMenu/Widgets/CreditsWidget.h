#pragma once

#include "TDCore.h"

#include "Blueprint/UserWidget.h"

#include "CreditsWidget.generated.h"

class UButton;
class UScrollBox;
class UVerticalBox;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCreditsBackClicked);

UCLASS(Abstract)
class UCreditsWidget : public UUserWidget
{
    GENERATED_BODY()
public:
    virtual bool Initialize() override;

    UPROPERTY(BlueprintAssignable)
    FOnCreditsBackClicked OnBackClicked;

protected:
    UFUNCTION()
    virtual void ReturnToPreviousScreen() const;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UScrollBox* ScrollBox;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UVerticalBox* VerticalBox;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UButton* BackButton;
};
