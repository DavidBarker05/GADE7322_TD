#include "UI/TowerDefence/TowerDefenceHUD.h"

#include "Blueprint/UserWidget.h"
#include "UI/TowerDefence/Widgets/LoseScreenWidget.h"
#include "UI/TowerDefence/Widgets/PauseScreenWidget.h"
#include "UI/TowerDefence/Widgets/PlayerHUDWidget.h"

void ATowerDefenceHUD::BeginPlay()
{
    Super::BeginPlay();
    APlayerController* PC = GetOwningPlayerController();
    FInputModeGameAndUI InputMode;
    InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);
    InputMode.SetHideCursorDuringCapture(false);
    PC->SetInputMode(InputMode);
    PC->bShowMouseCursor = true;
    PlayerHUDWidgetInstance = CreateWidget<UPlayerHUDWidget>(PC, PlayerHUDWidgetClass);
    PauseScreenWidgetInstance = CreateWidget<UPauseScreenWidget>(PC, PauseScreenWidgetClass);
    LoseScreenWidgetInstance = CreateWidget<ULoseScreenWidget>(PC, LoseScreenWidgetClass);
    ShowPlayerHUD();
}

void ATowerDefenceHUD::ShowPlayerHUD() const
{
    if (!PlayerHUDWidgetInstance) return;
    if (!PlayerHUDWidgetInstance->IsInViewport()) PlayerHUDWidgetInstance->AddToViewport();
}

void ATowerDefenceHUD::HidePlayerHUD() const
{
    if (!PlayerHUDWidgetInstance) return;
    if (PlayerHUDWidgetInstance->IsInViewport()) PlayerHUDWidgetInstance->RemoveFromParent();
}

void ATowerDefenceHUD::ShowPauseScreen() const
{
    if (!PauseScreenWidgetInstance) return;
    if (!PauseScreenWidgetInstance->IsInViewport()) PauseScreenWidgetInstance->AddToViewport();
}

void ATowerDefenceHUD::HidePauseScreen() const
{
    if (!PauseScreenWidgetInstance) return;
    if (PauseScreenWidgetInstance->IsInViewport()) PauseScreenWidgetInstance->RemoveFromParent();
}

void ATowerDefenceHUD::ShowLoseScreen() const
{
    if (!LoseScreenWidgetInstance) return;
    if (!LoseScreenWidgetInstance->IsInViewport()) LoseScreenWidgetInstance->AddToViewport();
}
