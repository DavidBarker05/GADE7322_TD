#pragma once

#include "CoreMinimal.h"

#include "Blueprint/UserWidget.h"
#include "Events/EventListener.h"

#include "PawnManagerWidget.generated.h"

class ADefender;
class ADefenderSpot;
class APlayerTower;
class UButton;
class UDefenderShopEntryWidget;
class UScrollBox;
class UTextBlock;
class UWidget;

// Part of the HUD. Hidden until the player selects a DefenderSpot, Defender or the PlayerTower, then shows
// buy/sell/heal/switch controls and stats for whatever is selected
UCLASS()
class GADE7322_TD_API UPawnManagerWidget : public UUserWidget,
                                           public IEventListener
{
    GENERATED_BODY()

    EVENTS_TO_LISTEN_TO(TEXT("UpdateHUDEvent"))

public:
    virtual bool Initialize() override;

protected:
    virtual void NativeOnInitialized() override;

    virtual void NativeDestruct() override;

    virtual void NativeTick(const FGeometry& MyGeometry, float DeltaTime) override;

public:
    UFUNCTION(BlueprintCallable)
    virtual void OnEventReceived_Implementation(const FName& EventName, const TArray<FAny>& Params) override;

protected:
    void SetTarget(AActor* NewTarget);

    void ClearTarget();

    void RefreshDisplay();

    void ShowDefenderInfo(ADefender* Defender);

    void ShowTowerInfo(APlayerTower* Tower);

    void ShowShop(ADefenderSpot* Spot);

    void PopulateShop();

    void RefreshShopAffordability();

    UFUNCTION()
    void HandleSellDefender() const;

    UFUNCTION()
    void HandleSwitchTarget();

    UFUNCTION()
    void HandleHealTower() const;

    UFUNCTION()
    void HandleBuyDefender(TSubclassOf<ADefender> DefenderClass) const;

    // Defenders that can be purchased from an unoccupied DefenderSpot
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shop", meta = (AllowPrivateAccess = true))
    TArray<TSubclassOf<ADefender>> AvailableDefenders;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shop", meta = (AllowPrivateAccess = true))
    TSubclassOf<UDefenderShopEntryWidget> DefenderShopEntryWidgetClass;

    // Shown when the selected DefenderSpot has a Defender, or a Defender is selected directly
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UWidget* DefenderInfoPanel;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UTextBlock* DefenderNameText;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UTextBlock* DefenderHealthText;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UTextBlock* DefenderDamageText;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UButton* SellButton;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UTextBlock* SellAmountText;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UButton* SwitchButton;

    // Shown when an unoccupied DefenderSpot is selected
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UWidget* ShopPanel;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UScrollBox* DefenderShopScrollBox;

    // Shown when the PlayerTower is selected
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UWidget* TowerPanel;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UTextBlock* TowerNameText;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UTextBlock* TowerHealthText;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UTextBlock* TowerDamageText;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UButton* HealButton;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UTextBlock* HealButtonText;

private:
    TWeakObjectPtr<AActor> CurrentTarget;

    UPROPERTY()
    ADefender* CurrentDisplayedDefender = nullptr;

    UPROPERTY()
    APlayerTower* CurrentDisplayedTower = nullptr;

    UPROPERTY()
    ADefenderSpot* CurrentShopSpot = nullptr;

    UPROPERTY()
    TArray<UDefenderShopEntryWidget*> ShopEntries;

    int32 CachedGold = 0;
};
