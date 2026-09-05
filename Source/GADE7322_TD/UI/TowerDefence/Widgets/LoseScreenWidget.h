#pragma once

#include "CoreMinimal.h"

#include "Blueprint/UserWidget.h"

#include "LoseScreenWidget.generated.h"

class UButton;

UCLASS(Abstract)
class GADE7322_TD_API ULoseScreenWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual bool Initialize() override;

protected:
    UFUNCTION()
    virtual void RestartGame() const;

    UFUNCTION()
    virtual void QuitGame() const;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UButton* RestartButton;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UButton* QuitButton;
};
