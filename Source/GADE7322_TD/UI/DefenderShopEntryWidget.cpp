#include "UI/DefenderShopEntryWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "TowerDefencePawns/Defenders/Defender.h"

bool UDefenderShopEntryWidget::Initialize()
{
    if (!Super::Initialize()) return false;
    if (!(DefenderNameText && BuyButton && BuyButtonText)) return false;
    BuyButton->OnClicked.AddDynamic(this, &UDefenderShopEntryWidget::HandleBuyClicked);
    return true;
}

void UDefenderShopEntryWidget::Setup(TSubclassOf<ADefender> InDefenderClass, int32 CurrentGold)
{
    DefenderClass = InDefenderClass;
    Cost = 0;
    if (const ADefender* DefaultDefender = InDefenderClass ? InDefenderClass->GetDefaultObject<ADefender>() : nullptr)
    {
        DefenderNameText->SetText(FText::FromName(DefaultDefender->GetPawnDisplayName()));
        Cost = DefaultDefender->GetCost();
    }
    BuyButtonText->SetText(FText::FromString(FString::Printf(TEXT("Buy for: %d Gold"), Cost)));
    RefreshAffordability(CurrentGold);
}

void UDefenderShopEntryWidget::RefreshAffordability(int32 CurrentGold) const
{
    BuyButton->SetIsEnabled(CurrentGold >= Cost);
}

void UDefenderShopEntryWidget::HandleBuyClicked() const { OnBuyClicked.Broadcast(DefenderClass); }
