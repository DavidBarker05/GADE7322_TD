#include "Player/TowerDefencePlayer.h"

#include "Camera/CameraComponent.h"
#include "CustomLog.h"
#include "EnhancedInputComponent.h"
#include "Framework/Application/SlateApplication.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "InputActionValue.h"
#include "Player/Components/CurrencyComponent.h"
#include "TowerDefencePawns/Defenders/DefenderSpot.h"
#include "TowerDefencePawns/Tower/PlayerTower.h"
#include "Widgets/SViewport.h"

ATowerDefencePlayer::ATowerDefencePlayer()
{
    PrimaryActorTick.bCanEverTick = true;
    FloatingPawnMovement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("Floating Pawn Movement"));
    CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    CameraComponent->SetupAttachment(RootComponent);
    CurrencyComponent = CreateDefaultSubobject<UCurrencyComponent>(TEXT("Currency Component"));
}

void ATowerDefencePlayer::BeginPlay()
{
    Super::BeginPlay();
    SUBSCRIBE_TO_EVENTS();
}

void ATowerDefencePlayer::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    UNSUBSCRIBE_FROM_EVENTS();
    Super::EndPlay(EndPlayReason);
}

void ATowerDefencePlayer::Tick(float DeltaTime) { Super::Tick(DeltaTime); }

void ATowerDefencePlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ATowerDefencePlayer::Move);
        EnhancedInputComponent->BindAction(MouseClickAction, ETriggerEvent::Started, this,
                                           &ATowerDefencePlayer::DoMouseClick);
    }
}

void ATowerDefencePlayer::OnEventReceived_Implementation(const FName& EventName, const TArray<FAny>& Params)
{
    if (EVENT_MATCHES(TEXT("DeathEvent"), 1) && PARAMS_ARE_VALID && PARAMS_ARE_CORRECT_TYPES(APlayerTower))
        TD_LOG_INFO(TEXT("Player is dead"));
}

void ATowerDefencePlayer::Move(const FInputActionValue& Value)
{
    FVector2D MovementVector = Value.Get<FVector2D>();
    FVector RightVector = GetActorRightVector() * MovementVector.X;
    FVector ForwardVector = GetActorForwardVector() * MovementVector.Y;
    FloatingPawnMovement->AddInputVector(RightVector + ForwardVector);
}

bool ATowerDefencePlayer::IsMouseOverUI(const APlayerController* PlayerController) const
{
    if (!PlayerController || !FSlateApplication::IsInitialized() || !GEngine || !GEngine->GameViewport) return false;

    float MouseX, MouseY;
    if (!PlayerController->GetMousePosition(MouseX, MouseY)) return false;

    const TSharedPtr<SViewport> GameViewportWidget = GEngine->GameViewport->GetGameViewportWidget();
    if (!GameViewportWidget.IsValid()) return false;

    const FWidgetPath WidgetPath = FSlateApplication::Get().LocateWindowUnderMouse(
        FVector2D(MouseX, MouseY), FSlateApplication::Get().GetInteractiveTopLevelWindows());

    return WidgetPath.IsValid() && WidgetPath.GetLastWidget() != GameViewportWidget;
}

void ATowerDefencePlayer::DoMouseClick()
{
    if (const APlayerController* PlayerController = Cast<APlayerController>(GetController()))
    {
        if (IsMouseOverUI(PlayerController)) return;

        FHitResult HitResult;
        if (!PlayerController->GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel1),
                                                                true, HitResult))
            return;

        if (ADefenderSpot* DefenderSpot = Cast<ADefenderSpot>(HitResult.GetActor()))
        {
            TD_LOG_INFO(TEXT("Clicked on spot"));
        }
    }
}
