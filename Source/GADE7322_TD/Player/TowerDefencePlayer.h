#pragma once

#include "CoreMinimal.h"

#include "Events/EventListener.h"
#include "GameFramework/Pawn.h"

#include "TowerDefencePlayer.generated.h"

class UCameraComponent;
class UCurrencyComponent;
class UFloatingPawnMovement;
class UInputComponent;
class UInputAction;
struct FInputActionValue;

UCLASS(Abstract)
class GADE7322_TD_API ATowerDefencePlayer : public APawn,
                                            public IEventListener
{
    GENERATED_BODY()

    EVENTS_TO_LISTEN_TO(TEXT("DeathEvent"))

public:
    ATowerDefencePlayer();

protected:
    virtual void BeginPlay() override;

    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
    virtual void Tick(float DeltaTime) override;

    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

public:
    UFUNCTION(BlueprintCallable)
    virtual void OnEventReceived_Implementation(const FName& EventName, const TArray<FAny>& Params) override;

public:
    void Move(const FInputActionValue& Value);

    void DoMouseClick();

private:
    // True if the cursor is currently over a hit-testable UMG widget rather than the game viewport.
    bool IsMouseOverUI(const APlayerController* PlayerController) const;

protected:
    UPROPERTY(EditAnywhere, Category = "Input")
    UInputAction* MoveAction;

    UPROPERTY(EditAnywhere, Category = "Input")
    UInputAction* MouseClickAction;

private:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    UFloatingPawnMovement* FloatingPawnMovement;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    UCameraComponent* CameraComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    UCurrencyComponent* CurrencyComponent;
};
