#pragma once

#include "TDCore.h"

#include "Blueprint/UserWidget.h"

#include "PawnManagerWidget.generated.h"

class ADefender;
class ADefenderSpot;
class APlayerTower;
class UButton;
class UDefenderInfoEntryWidget;
class UDefenderShopEntryWidget;
class UScrollBox;
class UTextBlock;
class UVerticalBox;
class UWidget;

UCLASS(Abstract)
class GADE7322_TD_API UPawnManagerWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual bool Initialize() override;

protected:
    virtual void NativeTick(const FGeometry& MyGeometry, float DeltaTime) override;

public:
    void SetTarget(AActor* NewTarget);

    // Also self-hides (unlike SetTarget, which leaves visibility to the caller)
    void ClearTarget();

    void UpdateGold(int32 NewGold);

protected:
    void RefreshDisplay();

    void ShowDefenderInfo(ADefender* Defender);

    void ShowTowerInfo(APlayerTower* Tower);

    void ShowShop(ADefenderSpot* Spot);

    // Covers the shop/info panel until the close button is pressed, then RefreshDisplay() brings back whichever of
    // them the current target needs
    void ShowAllDefendersInfo();

    void PopulateShop();

    void PopulateInfo();

    void RefreshShopAffordability();

    // Updates the next upgrade text and the upgrade button for the defender being displayed
    void RefreshUpgradeDisplay(const ADefender* Defender);

    UFUNCTION()
    void HandleUpgradeDefender() const;

    UFUNCTION()
    void HandleShowAllDefendersInfo();

    UFUNCTION()
    void HandleCloseAllDefendersInfo();

    UFUNCTION()
    void HandleSellDefender() const;

    UFUNCTION()
    void HandleSwitchTarget();

    UFUNCTION()
    void HandleHealTower() const;

    UFUNCTION()
    void HandleBuyDefender(TSubclassOf<ADefender> DefenderClass) const;

    // Defenders that can be purchased from an unoccupied DefenderSpot
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Defenders", meta = (AllowPrivateAccess = true))
    TArray<TSubclassOf<ADefender>> AvailableDefenders;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Defenders|Shop", meta = (AllowPrivateAccess = true))
    TSubclassOf<UDefenderShopEntryWidget> DefenderShopEntryWidgetClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Defenders|Info", meta = (AllowPrivateAccess = true))
    TSubclassOf<UDefenderInfoEntryWidget> DefenderInfoEntryWidgetClass;

    // Shown when the selected DefenderSpot has a Defender, or a Defender is selected directly
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UWidget* DefenderInfoPanel;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UButton* InfoPanelAllDefendersInfoButton;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UTextBlock* DefenderNameText;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UTextBlock* DefenderHealthText;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UTextBlock* DefenderDamageText;

    // Name of the next upgrade, or "Max level"
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UTextBlock* NextUpgradeText;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UButton* UpgradeButton;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UTextBlock* UpgradeButtonText;

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
    UButton* ShopPanelAllDefendersInfoButton;

    // Just the scrollable outer frame
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UScrollBox* DefenderShopScrollBox;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UVerticalBox* DefenderShopEntriesBox;

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

    // Display info about all defenders
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UWidget* AllDefendersInfoPanel;

    // Just the scrollable outer frame
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UScrollBox* AllDefendersInfoScrollBox;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UVerticalBox* DefenderInfoEntriesBox;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UButton* DefenderInfoCloseButton;

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

    UPROPERTY()
    TArray<UDefenderInfoEntryWidget*> InfoEntries;

    int32 CachedGold = 0;

    bool bShowingAllDefendersInfo = false;
};
