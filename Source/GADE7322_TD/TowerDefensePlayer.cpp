#include "TowerDefensePlayer.h"

#include "EnhancedInputComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "InputActionValue.h"
#include "Camera/CameraComponent.h"

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
        if (const AActor* DeadActor = Params[0].Get<AActor>())
        {
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
