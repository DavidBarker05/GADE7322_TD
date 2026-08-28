#include "Player/TowerDefencePlayerController.h"

#include "Engine/LocalPlayer.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"

void ATowerDefencePlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();
    if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
            ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
    {
        for (UInputMappingContext* CurrentContext : DefaultMappingContexts)
            Subsystem->AddMappingContext(CurrentContext, 0);
    }
}
