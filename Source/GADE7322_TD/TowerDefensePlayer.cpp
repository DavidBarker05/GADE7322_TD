#include "TowerDefensePlayer.h"

#include "EnhancedInputComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "InputActionValue.h"
#include "Camera/CameraComponent.h"
#include "Pawns/PlayerTower.h"
#include "Pawns/EnemyTroop.h"
#include "CustomLog.h"
#include "PlayerTroop.h"

ATowerDefensePlayer::ATowerDefensePlayer()
{
    PrimaryActorTick.bCanEverTick = true;
    FloatingPawnMovement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("Floating Pawn Movement"));
    CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    CameraComponent->SetupAttachment(RootComponent);
}

void ATowerDefensePlayer::BeginPlay()
{
    Super::BeginPlay();
    SUBSCRIBE_TO_EVENTS();
    CurrentCurrency = StartingCurrency;
}

void ATowerDefensePlayer::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
    UNSUBSCRIBE_FROM_EVENTS();
}

void ATowerDefensePlayer::Tick(float DeltaTime) { Super::Tick(DeltaTime); }

void ATowerDefensePlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ATowerDefensePlayer::Move);
    }
}

void ATowerDefensePlayer::OnEventReceived_Implementation(FName EventName, const TArray<FAny>& Params)
{
    if (EVENT_MATCHES(TEXT("DeathEvent"), 1) && PARAMS_ARE_VALID && PARAMS_ARE_CORRECT_TYPES(AActor))
    {
        if (const ATowerDefensePawn* DeadPawn = Params[0].Get<ATowerDefensePawn>())
        {
            if (!IsValid(DeadPawn)) return;
            if (DeadPawn->IsA<APlayerTower>())
            {
                TD_LOG_INFO(TEXT("Player is dead"));
            }
            else if (DeadPawn->IsA<APlayerTroop>())
            {
                TD_LOG_INFO(TEXT("%s died"), *DeadPawn->GetPawnDisplayName().ToString());
            }
            else if (const AEnemyTroop* EnemyTroop = Cast<AEnemyTroop>(DeadPawn))
            {
                TD_LOG_INFO(TEXT("%s died. Earned %d currency"), *EnemyTroop->GetPawnDisplayName().ToString(), EnemyTroop->GetCurrencyOnDeath());
                CurrentCurrency += EnemyTroop->GetCurrencyOnDeath();
            }
        }
    }
}

void ATowerDefensePlayer::Move(const FInputActionValue& Value)
{
    FVector2D MovementVector = Value.Get<FVector2D>();
    FVector RightVector = GetActorRightVector() * MovementVector.X;
    FVector ForwardVector = GetActorForwardVector() * MovementVector.Y;
    FloatingPawnMovement->AddInputVector(RightVector + ForwardVector);
}
