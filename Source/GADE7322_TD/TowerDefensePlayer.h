#pragma once

#include "CoreMinimal.h"

#include "EventListener.h"
#include "GameFramework/Pawn.h"

#include "TowerDefensePlayer.generated.h"

class UCameraComponent;
class UFloatingPawnMovement;
class UInputComponent;
class UInputAction;
struct FInputActionValue;

UCLASS(Abstract)
class GADE7322_TD_API ATowerDefensePlayer : public APawn,
                                            public IEventListener
{
    GENERATED_BODY()

    EVENTS_TO_LISTEN_TO(TEXT("DeathEvent"))

public:
    ATowerDefensePlayer();

protected:
    virtual void BeginPlay() override;

    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
    virtual void Tick(float DeltaTime) override;

    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

public:
    UFUNCTION(BlueprintCallable)
    virtual void OnEventReceived_Implementation(FName EventName, const TArray<FAny>& Params) override;

public:
    void Move(const FInputActionValue& Value);

protected:
    UPROPERTY(EditAnywhere, Category = "Input")
    UInputAction* MoveAction;

private:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    UFloatingPawnMovement* FloatingPawnMovement;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    UCameraComponent* CameraComponent;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Currency",
              meta = (AllowPrivateAccess = "true", ClampMin = "0", UIMin = "0"))
    int32 StartingCurrency = 0;

    UPROPERTY(BlueprintReadOnly, Category = "Currency", meta = (AllowPrivateAccess = "true"))
    int32 CurrentCurrency = 0;
};
