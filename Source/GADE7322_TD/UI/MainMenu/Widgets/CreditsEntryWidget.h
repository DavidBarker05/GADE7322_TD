#pragma once

#include "TDCore.h"

#include "Blueprint/UserWidget.h"

#include "CreditsEntryWidget.generated.h"

class UButton;
class UTextBlock;
class UVerticalBox;

UCLASS(Abstract)
class UCreditsEntryWidget : public UUserWidget
{
    GENERATED_BODY()
public:
    virtual bool Initialize() override;

protected:
    UFUNCTION()
    void OpenLink();

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UVerticalBox* VerticalBox;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UTextBlock* NameText;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UTextBlock* CreatorText;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UButton* LinkButton;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UTextBlock* LinkButtonText;

    UPROPERTY(EditAnywhere)
    FString EntryName;

    UPROPERTY(EditAnywhere)
    FString EntryCreator;

    UPROPERTY(EditAnywhere)
    FString URL;
};
