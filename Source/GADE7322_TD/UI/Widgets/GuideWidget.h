#pragma once

#include "TDCore.h"

#include "Blueprint/UserWidget.h"

#include "GuideWidget.generated.h"

class UButton;
class UDataTable;
class URichTextBlock;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGuideBackClicked);

UCLASS(Abstract)
class GADE7322_TD_API UGuideWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual bool Initialize() override;

    UPROPERTY(BlueprintAssignable)
    FOnGuideBackClicked OnBackClicked;

protected:
    UFUNCTION()
    virtual void ReturnToPreviousScreen() const;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    UDataTable* TextStyleDataTable;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    URichTextBlock* GuideText;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UButton* BackButton;
};
