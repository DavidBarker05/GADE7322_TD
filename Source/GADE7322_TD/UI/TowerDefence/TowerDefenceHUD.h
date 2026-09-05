#pragma once

#include "CoreMinimal.h"

#include "GameFramework/HUD.h"

#include "TowerDefenceHUD.generated.h"

class ULoseScreenWidget;
class UPauseScreenWidget;
class UPlayerHUDWidget;

UCLASS(Abstract)
class GADE7322_TD_API ATowerDefenceHUD : public AHUD
{
    GENERATED_BODY()

protected:
    virtual void BeginPlay() override;

public:
    UFUNCTION(BlueprintCallable, Category = "UI")
    void ShowPlayerHUD() const;

    UFUNCTION(BlueprintCallable, Category = "UI")
    void HidePlayerHUD() const;

    UFUNCTION(BlueprintCallable, Category = "UI")
    void ShowPauseScreen() const;

    UFUNCTION(BlueprintCallable, Category = "UI")
    void HidePauseScreen() const;

    UFUNCTION(BlueprintCallable, Category = "UI")
    void ShowLoseScreen() const;

private:
    UPROPERTY(EditDefaultsOnly, Category = "UI", meta = (AllowPrivateAccess = true))
    TSubclassOf<UPauseScreenWidget> PauseScreenWidgetClass;

    UPROPERTY(BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = true))
    UPauseScreenWidget* PauseScreenWidgetInstance;

    UPROPERTY(EditDefaultsOnly, Category = "UI", meta = (AllowPrivateAccess = true))
    TSubclassOf<UPlayerHUDWidget> PlayerHUDWidgetClass;

    UPROPERTY(BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = true))
    UPlayerHUDWidget* PlayerHUDWidgetInstance;

    UPROPERTY(EditDefaultsOnly, Category = "UI", meta = (AllowPrivateAccess = true))
    TSubclassOf<ULoseScreenWidget> LoseScreenWidgetClass;

    UPROPERTY(BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = true))
    ULoseScreenWidget* LoseScreenWidgetInstance;
};
