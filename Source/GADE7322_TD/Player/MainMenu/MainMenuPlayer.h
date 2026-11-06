#pragma once

#include "TDCore.h"

#include "GameFramework/Pawn.h"

#include "MainMenuPlayer.generated.h"

class UCameraComponent;

UCLASS(Abstract)
class GADE7322_TD_API AMainMenuPlayer : public APawn
{
    GENERATED_BODY()

public:
    AMainMenuPlayer();

    const UCameraComponent* GetCameraComponent() const { return CameraComponent; }
    UCameraComponent* GetCameraComponent() { return CameraComponent; }

private:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = true))
    UCameraComponent* CameraComponent;
};
