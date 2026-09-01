#pragma once

#include "CoreMinimal.h"

#include "Blueprint/UserWidget.h"

#include "ControlsWidget.generated.h"

class UTextBlock;
class UButton;

UCLASS()
class GADE7322_TD_API UControlsWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual bool Initialize() override;

protected:
    UFUNCTION()
    virtual void ReturnToPauseScreen() const;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UTextBlock* ControlsText;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UButton* BackButton;
};
