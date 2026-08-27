#include "Player/Components/CurrencyComponent.h"

UCurrencyComponent::UCurrencyComponent() { PrimaryComponentTick.bCanEverTick = false; }

void UCurrencyComponent::BeginPlay()
{
    Super::BeginPlay();
    CurrentCurrency = StartingCurrency;
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
