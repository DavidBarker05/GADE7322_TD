#include "UI/MainMenu/MainMenuHUD.h"

#include "Blueprint/UserWidget.h"
#include "UI/MainMenu/Widgets/MainMenuWidget.h"

void AMainMenuHUD::BeginPlay()
{
    Super::BeginPlay();
    APlayerController* PC = GetOwningPlayerController();
    FInputModeGameAndUI InputMode;
    InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);
    InputMode.SetHideCursorDuringCapture(false);
    PC->SetInputMode(InputMode);
    PC->bShowMouseCursor = true;
    MainMenuWidgetInstance = CreateWidget<UMainMenuWidget>(PC, MainMenuWidgetClass);
    LoadingScreenWidgetInstance = CreateWidget<UUserWidget>(PC, LoadingScreenWidgetClass);
    ShowMainMenu();
}

void AMainMenuHUD::ShowMainMenu() const
{
    if (!MainMenuWidgetInstance) return;
    if (!MainMenuWidgetInstance->IsInViewport()) MainMenuWidgetInstance->AddToViewport();
}

void AMainMenuHUD::ShowLoadingScreen() const
{
    if (!LoadingScreenWidgetInstance) return;
    if (!LoadingScreenWidgetInstance->IsInViewport()) LoadingScreenWidgetInstance->AddToViewport();
}
