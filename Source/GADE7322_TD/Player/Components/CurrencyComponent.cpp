// ReSharper disable CppParameterMayBeConst
#include "Player/Components/CurrencyComponent.h"

UCurrencyComponent::UCurrencyComponent() { PrimaryComponentTick.bCanEverTick = false; }

void UCurrencyComponent::BeginPlay()
{
    Super::BeginPlay();
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
void UCurrencyComponent::ResetCurrency()
{
    CurrentCurrency = StartingCurrency;
    BROADCAST_EVENT(TEXT("UpdateHUDEvent"), FName(TEXT("Currency")), CurrentCurrency);
}

void UCurrencyComponent::OnEventReceived_Implementation(const FName& EventName, const TArray<FAny>& Params)
{
    if ((EventName != TEXT("PurchaseEvent") && EventName != TEXT("SellEvent") &&
         EventName != TEXT("MoneyEarnedEvent")) ||
        Params.Num() != 1 || !PARAMS_ARE_VALID || !PARAMS_ARE_CORRECT_TYPES(int32))
        return;
    const int32 Amount = *Params[0].Get<int32>();
    if (EventName == TEXT("PurchaseEvent")) DecreaseCurrency(Amount);
    else IncreaseCurrency(Amount);
    BROADCAST_EVENT(TEXT("UpdateHUDEvent"), FName(TEXT("Currency")), CurrentCurrency);
}
