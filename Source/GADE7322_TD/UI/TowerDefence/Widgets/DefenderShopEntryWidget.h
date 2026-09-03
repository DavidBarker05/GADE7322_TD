#pragma once

#include "CoreMinimal.h"

#include "Blueprint/UserWidget.h"

#include "DefenderShopEntryWidget.generated.h"

class ADefender;
class UButton;
class UTextBlock;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDefenderBuyClicked, TSubclassOf<ADefender>, DefenderClass);

// A single buyable defender entry in a PawnManagerWidget's shop list
UCLASS(Abstract)
class GADE7322_TD_API UDefenderShopEntryWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual bool Initialize() override;

    void Setup(TSubclassOf<ADefender> InDefenderClass, int32 CurrentGold);

    void RefreshAffordability(int32 CurrentGold) const;

    UPROPERTY(BlueprintAssignable)
    FOnDefenderBuyClicked OnBuyClicked;

protected:
    UFUNCTION()
    void HandleBuyClicked() const;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UTextBlock* DefenderNameText;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UButton* BuyButton;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UTextBlock* BuyButtonText;

private:
    UPROPERTY()
    TSubclassOf<ADefender> DefenderClass;

    int32 Cost = 0;
};
