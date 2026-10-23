#include "UI/TowerDefence/Widgets/DefenderShopEntryWidget.h"

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
    if (TOptional<FName> DefenderName = ADefender::GetDefenderName(InDefenderClass, 0))
        DefenderNameText->SetText(FText::FromName(DefenderName.GetValue()));
    if (TOptional<int32> DefenderCost = ADefender::GetDefenderCost(InDefenderClass, 0)) Cost = DefenderCost.GetValue();
    else Cost = 0;
    BuyButtonText->SetText(FText::FromString(FString::Printf(TEXT("Buy for: %d Gold"), Cost)));
    RefreshAffordability(CurrentGold);
}

void UDefenderShopEntryWidget::RefreshAffordability(int32 CurrentGold) const
{
    BuyButton->SetIsEnabled(CurrentGold >= Cost);
}

void UDefenderShopEntryWidget::HandleBuyClicked() const { OnBuyClicked.Broadcast(DefenderClass); }
