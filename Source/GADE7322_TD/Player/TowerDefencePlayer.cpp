// ReSharper disable CppParameterMayBeConst
#include "Player/TowerDefencePlayer.h"

#include "Camera/CameraComponent.h"
#include "CustomLog.h"
#include "EnhancedInputComponent.h"
#include "Framework/Application/SlateApplication.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputActionValue.h"
#include "Player/Components/CurrencyComponent.h"
#include "TowerDefencePawns/Defenders/DefenderSpot.h"
#include "TowerDefencePawns/Tower/PlayerTower.h"
#include "Widgets/SViewport.h"

ATowerDefencePlayer::ATowerDefencePlayer()
{
    PrimaryActorTick.bCanEverTick = true;
    FloatingPawnMovement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("Floating Pawn Movement"));
    SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm Component"));
    SpringArmComponent->SetupAttachment(RootComponent);
    CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    CameraComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);
    CurrencyComponent = CreateDefaultSubobject<UCurrencyComponent>(TEXT("Currency Component"));
}

void ATowerDefencePlayer::BeginPlay()
{
    Super::BeginPlay();
    SUBSCRIBE_TO_EVENTS();
    CurrencyComponent->ResetCurrency();
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
        EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ATowerDefencePlayer::DoMove);
        EnhancedInputComponent->BindAction(RotateAction, ETriggerEvent::Triggered, this,
                                           &ATowerDefencePlayer::DoRotate);
        EnhancedInputComponent->BindAction(SelectAction, ETriggerEvent::Started, this, &ATowerDefencePlayer::DoSelect);
        EnhancedInputComponent->BindAction(DeselectAction, ETriggerEvent::Started, this,
                                           &ATowerDefencePlayer::DoDeselect);
        EnhancedInputComponent->BindAction(ZoomAction, ETriggerEvent::Triggered, this, &ATowerDefencePlayer::DoZoom);
        EnhancedInputComponent->BindAction(FocusAction, ETriggerEvent::Started, this, &ATowerDefencePlayer::DoFocus);
        EnhancedInputComponent->BindAction(PauseAction, ETriggerEvent::Started, this, &ATowerDefencePlayer::DoPause);
#if WITH_EDITOR
        EnhancedInputComponent->BindAction(PauseAction, ETriggerEvent::Started, this, &ATowerDefencePlayer::DoPause);
#endif
    }
}

void ATowerDefencePlayer::OnEventReceived_Implementation(const FName& EventName, const TArray<FAny>& Params)
{
    if (EVENT_MATCHES(TEXT("DeathEvent"), 1) && PARAMS_ARE_VALID && PARAMS_ARE_CORRECT_TYPES(APlayerTower))
        TD_LOG_INFO(TEXT("Player is dead"));
}

// ReSharper disable once CppMemberFunctionMayBeConst
void ATowerDefencePlayer::DoMove(const FInputActionValue& Value)
{
    const FVector2D MovementVector = Value.Get<FVector2D>();
    const FVector RightVector = GetActorRightVector() * MovementVector.X;
    const FVector ForwardVector = GetActorForwardVector() * MovementVector.Y;
    FloatingPawnMovement->AddInputVector(RightVector + ForwardVector);
}

void ATowerDefencePlayer::DoRotate(const FInputActionValue& Value) { }

bool ATowerDefencePlayer::IsMouseOverUI(const APlayerController* PlayerController)
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

// ReSharper disable once CppMemberFunctionMayBeConst
void ATowerDefencePlayer::DoSelect()
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

void ATowerDefencePlayer::DoDeselect() { }

void ATowerDefencePlayer::DoZoom(const FInputActionValue& Value) { }

void ATowerDefencePlayer::DoFocus() { }

void ATowerDefencePlayer::DoPause()
{
    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        bPaused = !bPaused;
        PC->SetPause(bPaused);
    }
}
