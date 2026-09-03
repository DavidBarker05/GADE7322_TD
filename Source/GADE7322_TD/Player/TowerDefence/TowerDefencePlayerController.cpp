#include "Player/TowerDefence/TowerDefencePlayerController.h"

#include "Engine/LocalPlayer.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "UI/TowerDefence/TowerDefenceHUD.h"

void ATowerDefencePlayerController::TogglePause()
{
    const bool bPaused = !IsPaused();
    SetPause(bPaused);
    if (const ATowerDefenceHUD* TD_HUD = GetHUD<ATowerDefenceHUD>())
    {
        if (bPaused) TD_HUD->ShowPauseScreen();
        else TD_HUD->HidePauseScreen();
    }
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
