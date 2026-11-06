#include "UI/TowerDefence/Widgets/PawnManagerWidget.h"

#include "Components/Button.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/Widget.h"
#include "Events/EventBus.h"
#include "Player/TowerDefence/TowerDefencePlayer.h"
#include "TowerDefencePawns/Components/DamageComponent.h"
#include "TowerDefencePawns/Components/HealthComponent.h"
#include "TowerDefencePawns/Defenders/Components/DefenderUpgradeComponent.h"
#include "TowerDefencePawns/Defenders/Defender.h"
#include "TowerDefencePawns/Defenders/DefenderSpot.h"
#include "TowerDefencePawns/Tower/PlayerTower.h"
#include "UI/TowerDefence/Widgets/DefenderInfoEntryWidget.h"
#include "UI/TowerDefence/Widgets/DefenderShopEntryWidget.h"

bool UPawnManagerWidget::Initialize()
{
    if (!Super::Initialize()) return false;
    if (!(DefenderInfoPanel && InfoPanelAllDefendersInfoButton && DefenderNameText && DefenderHealthText &&
          DefenderDamageText && NextUpgradeText && UpgradeButton && UpgradeButtonText && SellButton && SellAmountText &&
          SwitchButton && ShopPanel && ShopPanelAllDefendersInfoButton && DefenderShopScrollBox &&
          DefenderShopEntriesBox && TowerPanel && TowerNameText && TowerHealthText && TowerDamageText && HealButton &&
          HealButtonText && AllDefendersInfoPanel && AllDefendersInfoScrollBox && DefenderInfoEntriesBox &&
          DefenderInfoCloseButton))
        return false;
    SellButton->OnClicked.AddDynamic(this, &UPawnManagerWidget::HandleSellDefender);
    SwitchButton->OnClicked.AddDynamic(this, &UPawnManagerWidget::HandleSwitchTarget);
    HealButton->OnClicked.AddDynamic(this, &UPawnManagerWidget::HandleHealTower);
    UpgradeButton->OnClicked.AddDynamic(this, &UPawnManagerWidget::HandleUpgradeDefender);
    InfoPanelAllDefendersInfoButton->OnClicked.AddDynamic(this, &UPawnManagerWidget::HandleShowAllDefendersInfo);
    ShopPanelAllDefendersInfoButton->OnClicked.AddDynamic(this, &UPawnManagerWidget::HandleShowAllDefendersInfo);
    DefenderInfoCloseButton->OnClicked.AddDynamic(this, &UPawnManagerWidget::HandleCloseAllDefendersInfo);
    AllDefendersInfoPanel->SetVisibility(ESlateVisibility::Collapsed);
    ClearTarget();
    return true;
}

void UPawnManagerWidget::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
    Super::NativeTick(MyGeometry, DeltaTime);
    if (CurrentTarget.IsValid()) RefreshDisplay();
}

void UPawnManagerWidget::SetTarget(AActor* NewTarget)
{
    if (!IsValid(NewTarget))
    {
        ClearTarget();
        return;
    }
    if (CurrentTarget.Get() != NewTarget) bShowingAllDefendersInfo = false;
    CurrentTarget = NewTarget;
    RefreshDisplay();
}

void UPawnManagerWidget::ClearTarget()
{
    bShowingAllDefendersInfo = false;
    CurrentTarget = nullptr;
    CurrentDisplayedDefender = nullptr;
    CurrentDisplayedTower = nullptr;
    CurrentShopSpot = nullptr;
    SetVisibility(ESlateVisibility::Collapsed);
}

void UPawnManagerWidget::UpdateGold(int32 NewGold)
{
    CachedGold = NewGold;
    RefreshShopAffordability();
    if (IsValid(CurrentDisplayedTower))
        HealButton->SetIsEnabled(CachedGold >= CurrentDisplayedTower->GetHealCostPerPurchase());
    if (IsValid(CurrentDisplayedDefender)) RefreshUpgradeDisplay(CurrentDisplayedDefender);
}

void UPawnManagerWidget::RefreshDisplay()
{
    AActor* Target = CurrentTarget.Get();
    if (!IsValid(Target))
    {
        ClearTarget();
        return;
    }
    if (bShowingAllDefendersInfo)
    {
        ShowAllDefendersInfo();
        return;
    }
    AllDefendersInfoPanel->SetVisibility(ESlateVisibility::Collapsed);
    if (ADefenderSpot* Spot = Cast<ADefenderSpot>(Target))
    {
        if (Spot->IsOccupied()) ShowDefenderInfo(Spot->GetCurrentDefender());
        else ShowShop(Spot);
        return;
    }
    if (ADefender* Defender = Cast<ADefender>(Target))
    {
        if (!Defender->IsPawnActive() || !IsValid(Defender->GetSpawnPoint()))
        {
            ClearTarget();
            return;
        }
        ShowDefenderInfo(Defender);
        return;
    }
    if (APlayerTower* Tower = Cast<APlayerTower>(Target))
    {
        ShowTowerInfo(Tower);
        return;
    }
    ClearTarget();
}

void UPawnManagerWidget::ShowDefenderInfo(ADefender* Defender)
{
    if (!IsValid(Defender))
    {
        ClearTarget();
        return;
    }
    CurrentDisplayedDefender = Defender;
    CurrentDisplayedTower = nullptr;
    CurrentShopSpot = nullptr;
    ShopPanel->SetVisibility(ESlateVisibility::Collapsed);
    TowerPanel->SetVisibility(ESlateVisibility::Collapsed);
    DefenderInfoPanel->SetVisibility(ESlateVisibility::Visible);
    DefenderNameText->SetText(FText::FromName(Defender->GetPawnDisplayName()));
    const UHealthComponent* Health = Defender->GetHealthComponent();
    DefenderHealthText->SetText(FText::FromString(
        FString::Printf(TEXT("Health: %d / %d"), Health->GetCurrentHealth(), Health->GetBaseMaxHealth())));
    DefenderDamageText->SetText(
        FText::FromString(FString::Printf(TEXT("Damage: %d"), Defender->GetDamageComponent()->GetDamage())));
    SellAmountText->SetText(FText::FromString(FString::Printf(TEXT("Sell for: %d Gold"), Defender->GetSellPrice())));
    RefreshUpgradeDisplay(Defender);
}

void UPawnManagerWidget::ShowTowerInfo(APlayerTower* Tower)
{
    if (!IsValid(Tower))
    {
        ClearTarget();
        return;
    }
    CurrentDisplayedDefender = nullptr;
    CurrentDisplayedTower = Tower;
    CurrentShopSpot = nullptr;
    ShopPanel->SetVisibility(ESlateVisibility::Collapsed);
    DefenderInfoPanel->SetVisibility(ESlateVisibility::Collapsed);
    TowerPanel->SetVisibility(ESlateVisibility::Visible);
    TowerNameText->SetText(FText::FromName(Tower->GetPawnDisplayName()));
    const UHealthComponent* Health = Tower->GetHealthComponent();
    TowerHealthText->SetText(FText::FromString(
        FString::Printf(TEXT("Health: %d / %d"), Health->GetCurrentHealth(), Health->GetBaseMaxHealth())));
    TowerDamageText->SetText(
        FText::FromString(FString::Printf(TEXT("Damage: %d"), Tower->GetDamageComponent()->GetDamage())));
    HealButtonText->SetText(
        FText::FromString(FString::Printf(TEXT("Heal Tower: %d Gold"), Tower->GetHealCostPerPurchase())));
    HealButton->SetIsEnabled(CachedGold >= Tower->GetHealCostPerPurchase());
}

void UPawnManagerWidget::ShowShop(ADefenderSpot* Spot)
{
    if (!IsValid(Spot))
    {
        ClearTarget();
        return;
    }
    CurrentDisplayedDefender = nullptr;
    CurrentDisplayedTower = nullptr;
    DefenderInfoPanel->SetVisibility(ESlateVisibility::Collapsed);
    TowerPanel->SetVisibility(ESlateVisibility::Collapsed);
    ShopPanel->SetVisibility(ESlateVisibility::Visible);
    if (CurrentShopSpot != Spot)
    {
        CurrentShopSpot = Spot;
        PopulateShop();
    }
    else RefreshShopAffordability();
}

void UPawnManagerWidget::ShowAllDefendersInfo()
{
    DefenderInfoPanel->SetVisibility(ESlateVisibility::Collapsed);
    ShopPanel->SetVisibility(ESlateVisibility::Collapsed);
    TowerPanel->SetVisibility(ESlateVisibility::Collapsed);
    AllDefendersInfoPanel->SetVisibility(ESlateVisibility::Visible);
}

void UPawnManagerWidget::PopulateInfo()
{
    if (!DefenderInfoEntriesBox || !DefenderInfoEntryWidgetClass) return;
    DefenderInfoEntriesBox->ClearChildren();
    InfoEntries.Reset();
    for (const TSubclassOf<ADefender>& DefenderClass : AvailableDefenders)
    {
        if (!DefenderClass) continue;
        UDefenderInfoEntryWidget* Entry = CreateWidget<UDefenderInfoEntryWidget>(this, DefenderInfoEntryWidgetClass);
        if (!Entry) continue;
        Entry->Setup(DefenderClass);
        DefenderInfoEntriesBox->AddChildToVerticalBox(Entry);
        InfoEntries.Add(Entry);
    }
    if (AllDefendersInfoScrollBox) AllDefendersInfoScrollBox->ScrollToStart();
}

void UPawnManagerWidget::PopulateShop()
{
    if (!DefenderShopEntriesBox || !DefenderShopEntryWidgetClass) return;
    DefenderShopEntriesBox->ClearChildren();
    ShopEntries.Reset();
    for (const TSubclassOf<ADefender>& DefenderClass : AvailableDefenders)
    {
        if (!DefenderClass) continue;
        UDefenderShopEntryWidget* Entry = CreateWidget<UDefenderShopEntryWidget>(this, DefenderShopEntryWidgetClass);
        if (!Entry) continue;
        Entry->Setup(DefenderClass, CachedGold);
        Entry->OnBuyClicked.AddDynamic(this, &UPawnManagerWidget::HandleBuyDefender);
        DefenderShopEntriesBox->AddChildToVerticalBox(Entry);
        ShopEntries.Add(Entry);
    }
    if (DefenderShopScrollBox) DefenderShopScrollBox->ScrollToStart();
}

void UPawnManagerWidget::RefreshShopAffordability()
{
    for (UDefenderShopEntryWidget* Entry : ShopEntries)
        if (IsValid(Entry)) Entry->RefreshAffordability(CachedGold);
}

void UPawnManagerWidget::RefreshUpgradeDisplay(const ADefender* Defender)
{
    if (!IsValid(Defender)) return;
    const int32 NextLevel = Defender->GetUpgradeComponent()->GetCurrentLevel() + 1;
    const TOptional<FName> NextName = ADefender::GetDefenderName(Defender->GetClass(), NextLevel);
    const TOptional<int32> NextCost = ADefender::GetDefenderCost(Defender->GetClass(), NextLevel);
    if (!NextName || !NextCost)
    {
        NextUpgradeText->SetText(FText::FromString(TEXT("Max level")));
        UpgradeButtonText->SetText(FText::FromString(TEXT("Max level")));
        UpgradeButton->SetIsEnabled(false);
        return;
    }
    NextUpgradeText->SetText(
        FText::FromString(FString::Printf(TEXT("Next upgrade: %s"), *NextName.GetValue().ToString())));
    UpgradeButtonText->SetText(FText::FromString(FString::Printf(TEXT("Upgrade\n%d Gold"), NextCost.GetValue())));
    UpgradeButton->SetIsEnabled(CachedGold >= NextCost.GetValue());
}

void UPawnManagerWidget::HandleUpgradeDefender() const
{
    if (!IsValid(CurrentDisplayedDefender)) return;
    if (ADefenderSpot* Spot = CurrentDisplayedDefender->GetSpawnPoint(); IsValid(Spot))
    {
        const int32 NextLevel = CurrentDisplayedDefender->GetUpgradeComponent()->GetCurrentLevel() + 1;
        const TOptional<int32> Cost = ADefender::GetDefenderCost(CurrentDisplayedDefender->GetClass(), NextLevel);
        if (Cost && CachedGold >= Cost.GetValue()) Spot->UpgradeDefender();
    }
}

void UPawnManagerWidget::HandleShowAllDefendersInfo()
{
    if (InfoEntries.IsEmpty()) PopulateInfo();
    else if (AllDefendersInfoScrollBox) AllDefendersInfoScrollBox->ScrollToStart();
    bShowingAllDefendersInfo = true;
    RefreshDisplay();
}

void UPawnManagerWidget::HandleCloseAllDefendersInfo()
{
    bShowingAllDefendersInfo = false;
    RefreshDisplay();
}

void UPawnManagerWidget::HandleSellDefender() const
{
    if (!IsValid(CurrentDisplayedDefender)) return;
    if (ADefenderSpot* Spot = CurrentDisplayedDefender->GetSpawnPoint(); IsValid(Spot)) Spot->SellDefender();
}

void UPawnManagerWidget::HandleSwitchTarget()
{
    ATowerDefencePlayer* Player = GetOwningPlayerPawn<ATowerDefencePlayer>();
    if (!Player) return;
    Player->SwitchBetweenSpotAndDefender();
    SetTarget(Player->GetCurrentFocusTarget());
}

void UPawnManagerWidget::HandleHealTower() const
{
    if (!IsValid(CurrentDisplayedTower)) return;
    const int32 Cost = CurrentDisplayedTower->GetHealCostPerPurchase();
    if (CachedGold < Cost) return;
    BROADCAST_EVENT(TEXT("PurchaseEvent"), Cost);
    CurrentDisplayedTower->GetHealthComponent()->ReceiveHealth(CurrentDisplayedTower->GetHealAmountPerPurchase());
}

void UPawnManagerWidget::HandleBuyDefender(TSubclassOf<ADefender> DefenderClass) const
{
    if (!IsValid(CurrentShopSpot) || !DefenderClass) return;
    if (const TOptional<int32> DefenderCost = ADefender::GetDefenderCost(DefenderClass, 0);
        CachedGold < DefenderCost.GetValue())
        return;
    CurrentShopSpot->PurchaseDefender(DefenderClass);
}
