#include "UI/PlayerHUDWidget.h"

#include "Components/TextBlock.h"

void UPlayerHUDWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();
    SUBSCRIBE_TO_EVENTS();
}
void UPlayerHUDWidget::NativeDestruct()
{
    UNSUBSCRIBE_FROM_EVENTS();
    Super::NativeDestruct();
}

void UPlayerHUDWidget::OnEventReceived_Implementation(const FName& EventName, const TArray<FAny>& Params)
{
    if (EventName != TEXT("UpdateHUDEvent") || !PARAMS_ARE_VALID || Params.Num() < 1) return;
    const FName* ElemNamePtr = Params[0].Get<FName>();
    if (!ElemNamePtr) return;
    const FName ElemName = *ElemNamePtr;
    const uint32 NumParams = Params.Num();
    if (ElemName == TEXT("Round"))
    {
        if (NumParams != 2) return;
        if (const int32* Round = Params[1].Get<int32>())
            RoundDisplay->SetText(FText::FromString(FString::Printf(TEXT("Round: %d"), *Round)));
    }
    else if (ElemName == TEXT("EnemyCount"))
    {
        if (NumParams != 2) return;
        if (const int32* Count = Params[1].Get<int32>())
            EnemyCountDisplay->SetText(FText::FromString(FString::Printf(TEXT("Enemies Remaining: %d"), *Count)));
    }
    else if (ElemName == TEXT("Currency"))
    {
        if (NumParams != 2) return;
        if (const int32* Amount = Params[1].Get<int32>())
            CurrencyDisplay->SetText(FText::FromString(FString::Printf(TEXT("Gold: %d"), *Amount)));
    }
    else if (ElemName == TEXT("PlayerSelected"))
    {
        if (NumParams != 1) return;
    }
    else if (ElemName == TEXT("PlayerDeselected"))
    {
        if (NumParams != 1) return;
    }
}
