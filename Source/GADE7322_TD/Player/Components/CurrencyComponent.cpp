#include "Player/Components/CurrencyComponent.h"

UCurrencyComponent::UCurrencyComponent() { PrimaryComponentTick.bCanEverTick = false; }

void UCurrencyComponent::BeginPlay()
{
    Super::BeginPlay();
    CurrentCurrency = StartingCurrency;
    SUBSCRIBE_TO_EVENTS();
}

void UCurrencyComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    UNSUBSCRIBE_FROM_EVENTS();
    Super::EndPlay(EndPlayReason);
}

void UCurrencyComponent::IncreaseCurrency(int32 Amount)
{
    if (Amount > 0) CurrentCurrency += Amount;
}

void UCurrencyComponent::DecreaseCurrency(int32 Amount)
{
    if (Amount > 0) CurrentCurrency -= Amount;
}

void UCurrencyComponent::ResetCurrency() { CurrentCurrency = StartingCurrency; }

void UCurrencyComponent::OnEventReceived_Implementation(const FName& EventName, const TArray<FAny>& Params)
{
    if (EVENT_MATCHES(TEXT("PurchaseEvent"), 1) && PARAMS_ARE_VALID && PARAMS_ARE_CORRECT_TYPES(int32))
        DecreaseCurrency(*Params[0].Get<int32>());
    else if ((EVENT_MATCHES(TEXT("SellEvent"), 1) || EVENT_MATCHES(TEXT("MoneyEarnedEvent"), 1)) && PARAMS_ARE_VALID &&
             PARAMS_ARE_CORRECT_TYPES(int32))
        IncreaseCurrency(*Params[0].Get<int32>());
}
