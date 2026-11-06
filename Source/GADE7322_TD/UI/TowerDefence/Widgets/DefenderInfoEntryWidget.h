#pragma once

#include "TDCore.h"

#include "Blueprint/UserWidget.h"

#include "DefenderInfoEntryWidget.generated.h"

class ADefender;
class URichTextBlock;

UCLASS(Abstract)
class GADE7322_TD_API UDefenderInfoEntryWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual bool Initialize() override;

    void Setup(TSubclassOf<ADefender> InDefenderClass) const;

protected:
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    URichTextBlock* DefenderInfoText;
};
