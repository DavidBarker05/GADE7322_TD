#include "UI/PawnManagerWidget.h"

#include "Components/Button.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "Components/Widget.h"
#include "Player/TowerDefencePlayer.h"
#include "TowerDefencePawns/Components/DamageComponent.h"
#include "TowerDefencePawns/Components/HealthComponent.h"
#include "TowerDefencePawns/Defenders/Defender.h"
#include "TowerDefencePawns/Defenders/DefenderSpot.h"
#include "TowerDefencePawns/Tower/PlayerTower.h"
#include "UI/DefenderShopEntryWidget.h"

bool UPawnManagerWidget::Initialize()
{
    if (!Super::Initialize()) return false;
    if (!(DefenderInfoPanel && DefenderNameText && DefenderHealthText && DefenderDamageText && SellButton &&
          SellAmountText && SwitchButton && ShopPanel && DefenderShopScrollBox && TowerPanel && TowerNameText &&
          TowerHealthText && TowerDamageText && HealButton && HealButtonText))
        return false;
    SellButton->OnClicked.AddDynamic(this, &UPawnManagerWidget::HandleSellDefender);
    SwitchButton->OnClicked.AddDynamic(this, &UPawnManagerWidget::HandleSwitchTarget);
    HealButton->OnClicked.AddDynamic(this, &UPawnManagerWidget::HandleHealTower);
    ClearTarget();
    return true;
}

void UPawnManagerWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();
    SUBSCRIBE_TO_EVENTS();
}

void UPawnManagerWidget::NativeDestruct()
{
    UNSUBSCRIBE_FROM_EVENTS();
    Super::NativeDestruct();
}

void UPawnManagerWidget::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
    Super::NativeTick(MyGeometry, DeltaTime);
    if (CurrentTarget.IsValid()) RefreshDisplay();
}

void UPawnManagerWidget::OnEventReceived_Implementation(const FName& EventName, const TArray<FAny>& Params)
{
    if (EventName != TEXT("UpdateHUDEvent") || !PARAMS_ARE_VALID || Params.Num() < 1) return;
    const FName* ElemNamePtr = Params[0].Get<FName>();
    if (!ElemNamePtr) return;
    const FName ElemName = *ElemNamePtr;
    const int32 NumParams = Params.Num();
    if (ElemName == TEXT("Currency"))
    {
        if (NumParams != 2) return;
        if (const int32* Amount = Params[1].Get<int32>())
        {
            CachedGold = *Amount;
            RefreshShopAffordability();
        }
    }
    else if (ElemName == TEXT("PawnManager"))
    {
        if (NumParams < 2) return;
        const FName* ActionPtr = Params[1].Get<FName>();
        if (!ActionPtr) return;
        if (*ActionPtr == TEXT("Select"))
        {
            if (NumParams != 3) return;
            if (AActor* const* ActorPtr = Params[2].Get<AActor*>()) SetTarget(*ActorPtr);
        }
        else if (*ActionPtr == TEXT("Deselect"))
        {
            if (NumParams != 2) return;
            ClearTarget();
        }
    }
}

void UPawnManagerWidget::SetTarget(AActor* NewTarget)
{
    if (!IsValid(NewTarget))
    {
        ClearTarget();
        return;
    }
    CurrentTarget = NewTarget;
    SetVisibility(ESlateVisibility::Visible);
    RefreshDisplay();
}

void UPawnManagerWidget::ClearTarget()
{
    CurrentTarget = nullptr;
    CurrentDisplayedDefender = nullptr;
    CurrentDisplayedTower = nullptr;
    CurrentShopSpot = nullptr;
    SetVisibility(ESlateVisibility::Collapsed);
}

void UPawnManagerWidget::RefreshDisplay()
{
    AActor* Target = CurrentTarget.Get();
    if (!IsValid(Target))
    {
        ClearTarget();
        return;
    }
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
        FString::Printf(TEXT("Health: %d / %d"), Health->GetCurrentHealth(), Health->GetMaxHealth())));
    DefenderDamageText->SetText(
        FText::FromString(FString::Printf(TEXT("Damage: %d"), Defender->GetDamageComponent()->GetDamage())));
    SellAmountText->SetText(FText::FromString(FString::Printf(TEXT("Sell for: %d Gold"), Defender->GetSellPrice())));
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
        FString::Printf(TEXT("Health: %d / %d"), Health->GetCurrentHealth(), Health->GetMaxHealth())));
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

void UPawnManagerWidget::PopulateShop()
{
    if (!DefenderShopScrollBox || !DefenderShopEntryWidgetClass) return;
    DefenderShopScrollBox->ClearChildren();
    ShopEntries.Reset();
    for (const TSubclassOf<ADefender>& DefenderClass : AvailableDefenders)
    {
        if (!DefenderClass) continue;
        UDefenderShopEntryWidget* Entry = CreateWidget<UDefenderShopEntryWidget>(this, DefenderShopEntryWidgetClass);
        if (!Entry) continue;
        Entry->Setup(DefenderClass, CachedGold);
        Entry->OnBuyClicked.AddDynamic(this, &UPawnManagerWidget::HandleBuyDefender);
        DefenderShopScrollBox->AddChild(Entry);
        ShopEntries.Add(Entry);
    }
}

void UPawnManagerWidget::RefreshShopAffordability()
{
    for (UDefenderShopEntryWidget* Entry : ShopEntries)
        if (IsValid(Entry)) Entry->RefreshAffordability(CachedGold);
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
    if (const ADefender* DefaultDefender = DefenderClass->GetDefaultObject<ADefender>();
        !DefaultDefender || CachedGold < DefaultDefender->GetCost())
        return;
    CurrentShopSpot->PurchaseDefender(DefenderClass);
}
