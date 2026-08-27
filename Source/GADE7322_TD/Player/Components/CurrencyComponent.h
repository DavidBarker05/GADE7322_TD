#pragma once

#include "CoreMinimal.h"

#include "Components/ActorComponent.h"
#include "Events/EventListener.h"

#include "CurrencyComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class GADE7322_TD_API UCurrencyComponent : public UActorComponent,
                                           public IEventListener
{
    GENERATED_BODY()

    EVENTS_TO_LISTEN_TO(TEXT("PurchaseEvent"), TEXT("SellEvent"), TEXT("MoneyEarnedEvent"))

public:
    UCurrencyComponent();

protected:
    virtual void BeginPlay() override;

    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
    UFUNCTION(BlueprintCallable, Category = "Selling")
    void IncreaseCurrency(int32 Amount);

    UFUNCTION(BlueprintCallable, Category = "Purchasing")
    void DecreaseCurrency(int32 Amount);

    UFUNCTION(BlueprintCallable)
    void ResetCurrency();

    int32 GetStartingCurrency() const { return StartingCurrency; }

    int32 GetCurrentCurrency() const { return CurrentCurrency; }

public:
    UFUNCTION(BlueprintCallable)
    virtual void OnEventReceived_Implementation(const FName& EventName, const TArray<FAny>& Params) override;

private:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Currency",
              meta = (AllowPrivateAccess = true, ClampMin = "0", UIMin = "0"))
    int32 StartingCurrency = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Currency", meta = (AllowPrivateAccess = "true"))
    int32 CurrentCurrency = 0;
};
