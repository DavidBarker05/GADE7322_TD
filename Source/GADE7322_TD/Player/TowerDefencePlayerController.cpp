#include "Player/TowerDefencePlayerController.h"

#include "Blueprint/UserWidget.h"
#include "Engine/LocalPlayer.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "UI/PauseScreenWidget.h"
#include "UI/PlayerHUDWidget.h"

void ATowerDefencePlayerController::BeginPlay()
{
    Super::BeginPlay();
    FInputModeGameAndUI InputMode;
    InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);
    InputMode.SetHideCursorDuringCapture(false);
    SetInputMode(InputMode);
    bShowMouseCursor = true;
    if (!PlayerHUDWidgetInstance) PlayerHUDWidgetInstance = CreateWidget<UPlayerHUDWidget>(this, PlayerHUDWidgetClass);
    if (PlayerHUDWidgetInstance && !PlayerHUDWidgetInstance->IsInViewport()) PlayerHUDWidgetInstance->AddToViewport();
}

void ATowerDefencePlayerController::TogglePause()
{
    bIsPaused = !bIsPaused;
    SetPause(bIsPaused);
    if (bIsPaused)
    {
        if (!PauseScreenWidgetInstance && PauseScreenWidgetClass)
            PauseScreenWidgetInstance = CreateWidget<UPauseScreenWidget>(this, PauseScreenWidgetClass);
        if (PauseScreenWidgetInstance)
        {
            PauseScreenWidgetInstance->ShowMainPanel();
            if (!PauseScreenWidgetInstance->IsInViewport()) PauseScreenWidgetInstance->AddToViewport();
        }
    }
    else if (PauseScreenWidgetInstance) PauseScreenWidgetInstance->RemoveFromParent();
}

void ATowerDefencePlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();
    if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
            ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
    {
        for (const UInputMappingContext* CurrentContext : DefaultMappingContexts)
            Subsystem->AddMappingContext(CurrentContext, 0);
    }
}
