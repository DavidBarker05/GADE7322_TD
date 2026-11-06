#include "Player/MainMenu/MainMenuPlayer.h"

#include "Camera/CameraComponent.h"

AMainMenuPlayer::AMainMenuPlayer()
{
    PrimaryActorTick.bCanEverTick = false;
    SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("Root")));
    CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
}
