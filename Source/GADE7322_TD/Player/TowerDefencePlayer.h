#pragma once

#include "CoreMinimal.h"

#include "Events/EventListener.h"
#include "GameFramework/Pawn.h"

#include "TowerDefencePlayer.generated.h"

class ADefenderSpot;
struct FInputActionValue;
class UCameraComponent;
class UCurrencyComponent;
class UFloatingPawnMovement;
class UInputComponent;
class UInputAction;
class USpringArmComponent;

UCLASS(Abstract)
class GADE7322_TD_API ATowerDefencePlayer : public APawn,
                                            public IEventListener
{
    GENERATED_BODY()

    EVENTS_TO_LISTEN_TO(TEXT("DeathEvent"))

public:
    ATowerDefencePlayer();

    virtual void BeginPlay() override;

    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    virtual void Tick(float DeltaTime) override;

protected:
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

public:
    UFUNCTION(BlueprintCallable)
    virtual void OnEventReceived_Implementation(const FName& EventName, const TArray<FAny>& Params) override;

    UFUNCTION(BlueprintCallable)
    void SwitchBetweenSpotAndDefender();

    void DoMove(const FInputActionValue& Value);

    void DoRotate(const FInputActionValue& Value);

    void DoSelect();

    void DoDeselect();

    void DoZoom(const FInputActionValue& Value);

    void DoFocus();

    void DoReset();

    void DoPause();

private:
    static bool IsMouseOverUI(const APlayerController* PlayerController);

protected:
    UPROPERTY(EditAnywhere, Category = "Input")
    UInputAction* MoveAction;

    UPROPERTY(EditAnywhere, Category = "Input")
    UInputAction* RotateAction;

    UPROPERTY(EditAnywhere, Category = "Input")
    UInputAction* SelectAction;

    UPROPERTY(EditAnywhere, Category = "Input")
    UInputAction* DeselectAction;

    UPROPERTY(EditAnywhere, Category = "Input")
    UInputAction* ZoomAction;

    UPROPERTY(EditAnywhere, Category = "Input")
    UInputAction* FocusAction;

    UPROPERTY(EditAnywhere, Category = "Input")
    UInputAction* ResetAction;

    UPROPERTY(EditAnywhere, Category = "Input")
    UInputAction* PauseAction;

#if WITH_EDITORONLY_DATA
    UPROPERTY(EditAnywhere, Category = "Input")
    UInputAction* EditorPauseAction; // Because esc closes game so extra key to pause in editor
#endif

private:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
    USceneComponent* Root;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = true))
    UFloatingPawnMovement* FloatingPawnMovement;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = true))
    USpringArmComponent* SpringArmComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = true))
    UCameraComponent* CameraComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = true))
    UCurrencyComponent* CurrencyComponent;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Zoom",
              meta = (AllowPrivateAccess = true, ClampMin = 0.0, UIMin = 0.0, Units = "Centimeters"))
    float ClosestCameraDistance = 500.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Zoom",
              meta = (AllowPrivateAccess = true, ClampMin = 0.0, UIMin = 0.0, Units = "Centimeters"))
    float FurthestCameraDistance = 2000.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Zoom",
              meta = (AllowPrivateAccess = true, ClampMin = 0.0, UIMin = 0.0))
    float ZoomSpeed = 150.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rotation",
              meta = (AllowPrivateAccess = true, ClampMin = 0.0, UIMin = 0.0, Units = "deg/s"))
    float RotationSpeed = 90.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Selection", meta = (AllowPrivateAccess = true))
    const ADefenderSpot* CurrentlySelectedDefenderSpot;

    UPROPERTY(BlueprintReadOnly, Category = "Selection", meta = (AllowPrivateAccess = true))
    const AActor* CurrentFocusTarget;

    UPROPERTY(BlueprintReadOnly, Category = "Selection", meta = (AllowPrivateAccess = true))
    bool bFollowTarget = false;

    bool bPaused = false;
};
