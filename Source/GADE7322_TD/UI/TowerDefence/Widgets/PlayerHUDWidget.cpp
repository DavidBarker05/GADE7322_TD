#include "UI/TowerDefence/Widgets/PlayerHUDWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "TowerDefenceGameMode.h"

bool UPlayerHUDWidget::Initialize()
{
    if (!Super::Initialize()) return false;
    if (!(WaveButton && WaveButtonText)) return false;
    WaveButton->OnClicked.AddDynamic(this, &UPlayerHUDWidget::HandleWaveButtonClicked);
    return true;
}

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

void UPlayerHUDWidget::HandleWaveButtonClicked()
{
    if (ATowerDefenceGameMode* GameMode = GetWorld() ? GetWorld()->GetAuthGameMode<ATowerDefenceGameMode>() : nullptr)
        GameMode->StartNextWave();
}

void UPlayerHUDWidget::OnEventReceived_Implementation(const FName& EventName, const TArray<FAny>& Params)
{
    if (EventName != TEXT("UpdateHUDEvent") || !PARAMS_ARE_VALID || Params.Num() < 2) return;
    const FName* ElemNamePtr = Params[0].Get<FName>();
    if (!ElemNamePtr) return;
    const FName ElemName = *ElemNamePtr;
    const uint32 NumParams = Params.Num();
    if (ElemName == TEXT("Round"))
    {
        if (NumParams != 2) return;
        if (const int32* Round = Params[1].Get<int32>())
        {
            LastKnownRound = *Round;
            RoundDisplay->SetText(FText::FromString(FString::Printf(TEXT("Round: %d"), *Round)));
        }
    }
    else if (ElemName == TEXT("WaveState"))
    {
        if (NumParams != 2) return;
        if (const FName* StatePtr = Params[1].Get<FName>())
        {
            if (*StatePtr == TEXT("AwaitingStart"))
            {
                WaveButtonText->SetText(FText::FromString(LastKnownRound == 0 ? TEXT("Start") : TEXT("Next Wave")));
                WaveButton->SetVisibility(ESlateVisibility::Visible);
            }
            else if (*StatePtr == TEXT("InProgress")) WaveButton->SetVisibility(ESlateVisibility::Collapsed);
        }
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
    else if (ElemName == TEXT("PawnManager"))
    {
        const FName* ActionPtr = Params[1].Get<FName>();
        if (!ActionPtr) return;
        const FName Action = *ActionPtr;
        if (Action == TEXT("Select"))
        {
            if (NumParams != 3) return;
        }
        else if (Action == TEXT("Deselect"))
        {
            if (NumParams != 2) return;
        }
    }
}
