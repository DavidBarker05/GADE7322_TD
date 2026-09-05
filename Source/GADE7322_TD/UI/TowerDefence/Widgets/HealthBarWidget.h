#pragma once

#include "CoreMinimal.h"

#include "Blueprint/UserWidget.h"

#include "HealthBarWidget.generated.h"

class UProgressBar;

UCLASS(Abstract)
class GADE7322_TD_API UHealthBarWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual bool Initialize() override;

    const UProgressBar* GetHealthBar() const { return HealthBar; }
    UProgressBar* GetHealthBar() { return HealthBar; }

protected:
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UProgressBar* HealthBar;
};
