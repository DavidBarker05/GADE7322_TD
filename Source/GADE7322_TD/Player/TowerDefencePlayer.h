#pragma once

#include "CoreMinimal.h"

#include "Events/EventListener.h"
#include "GameFramework/Pawn.h"

#include "TowerDefencePlayer.generated.h"

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

    void DoMove(const FInputActionValue& Value);
    
    void DoRotate(const FInputActionValue& Value);

    void DoSelect();

    void DoDeselect();

    void DoZoom(const FInputActionValue& Value);

    void DoFocus();

    void DoPause();

private:
    // True if the cursor is currently over a hit-testable UMG widget rather than the game viewport
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
    UInputAction* PauseAction;

#if WITH_EDITORONLY_DATA
    UPROPERTY(EditAnywhere, Category = "Input")
    UInputAction* EditorPauseAction; // Because esc closes game so extra key to pause in editor
#endif

private:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = true))
    UFloatingPawnMovement* FloatingPawnMovement;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = true))
    USpringArmComponent* SpringArmComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = true))
    UCameraComponent* CameraComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = true))
    UCurrencyComponent* CurrencyComponent;

    bool bPaused = false;
};
