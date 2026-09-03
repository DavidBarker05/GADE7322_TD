#pragma once

#include "CoreMinimal.h"

#include "Blueprint/UserWidget.h"
#include "Events/EventListener.h"

#include "PlayerHUDWidget.generated.h"

class UButton;
class UPawnManagerWidget;
class UTextBlock;

UCLASS(Abstract)
class GADE7322_TD_API UPlayerHUDWidget : public UUserWidget,
                                         public IEventListener
{
    GENERATED_BODY()

    EVENTS_TO_LISTEN_TO(TEXT("UpdateHUDEvent"))

public:
    virtual bool Initialize() override;

protected:
    virtual void NativeOnInitialized() override;

    virtual void NativeDestruct() override;

public:
    virtual void OnEventReceived_Implementation(const FName& EventName, const TArray<FAny>& Params) override;

protected:
    UFUNCTION()
    void HandleWaveButtonClicked();

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UButton* PauseButton;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UTextBlock* RoundDisplay;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UTextBlock* EnemyCountDisplay;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UTextBlock* CurrencyDisplay;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UButton* WaveButton;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UTextBlock* WaveButtonText;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UPawnManagerWidget* PawnManagerWidget;

private:
    int32 LastKnownRound = 0;
};
