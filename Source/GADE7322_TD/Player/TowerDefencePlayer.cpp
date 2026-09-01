// ReSharper disable CppParameterMayBeConst
#include "Player/TowerDefencePlayer.h"

#include "Camera/CameraComponent.h"
#include "CustomLog.h"
#include "EnhancedInputComponent.h"
#include "Framework/Application/SlateApplication.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputActionValue.h"
#include "Player/Components/CurrencyComponent.h"
#include "Player/TowerDefencePlayerController.h"
#include "TowerDefencePawns/Defenders/Defender.h"
#include "TowerDefencePawns/Defenders/DefenderSpot.h"
#include "TowerDefencePawns/Tower/PlayerTower.h"
#include "Widgets/SViewport.h"

ATowerDefencePlayer::ATowerDefencePlayer()
{
    PrimaryActorTick.bCanEverTick = true;
    Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    SetRootComponent(Root);
    FloatingPawnMovement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("Floating Pawn Movement"));
    SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm Component"));
    SpringArmComponent->SetupAttachment(RootComponent);
    SpringArmComponent->TargetArmLength = FurthestCameraDistance; // Start fully zoomed out
    SpringArmComponent->bDoCollisionTest = false;
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

void ATowerDefencePlayer::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    // This will probably lag behind, but Unreal doesn't really have a late update
    // like unity. There are tick groups, but I'm not sure which group to go in.
    // Because either I miss physics or miss camera update and idk if either is safe
    if (!bFollowTarget) return;
    if (!IsValid(CurrentFocusTarget))
    {
        bFollowTarget = false;
        return;
    }
    FVector DesiredLocation = CurrentFocusTarget->GetActorLocation();
    DesiredLocation.Z = GetActorLocation().Z; // Height, I think... Unreal is weird
    SetActorLocation(DesiredLocation);
}

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

void ATowerDefencePlayer::SwitchBetweenSpotAndDefender()
{
    if (!IsValid(CurrentFocusTarget)) return;
    if (const ADefenderSpot* Spot = Cast<ADefenderSpot>(CurrentFocusTarget))
    {
        if (IsValid(Spot->GetCurrentDefender())) CurrentFocusTarget = Spot->GetCurrentDefender();
        else CurrentFocusTarget = nullptr;
    }
    else if (const ADefender* Defender = Cast<ADefender>(CurrentFocusTarget))
    {
        if (IsValid(Defender->GetSpawnPoint())) CurrentFocusTarget = Defender->GetSpawnPoint();
        else CurrentFocusTarget = nullptr;
    }
    else CurrentFocusTarget = nullptr;
}

// ReSharper disable once CppMemberFunctionMayBeConst
void ATowerDefencePlayer::DoMove(const FInputActionValue& Value)
{
    if (bPaused) return;
    const FVector2D MovementVector = Value.Get<FVector2D>();
    const FVector RightVector = GetActorRightVector() * MovementVector.X;
    const FVector ForwardVector = GetActorForwardVector() * MovementVector.Y;
    FloatingPawnMovement->AddInputVector(RightVector + ForwardVector);
    if (MovementVector.SquaredLength() > KINDA_SMALL_NUMBER) bFollowTarget = false;
}

// ReSharper disable once CppMemberFunctionMayBeConst
void ATowerDefencePlayer::DoRotate(const FInputActionValue& Value)
{
    if (bPaused) return;
    const float RotationInput = Value.Get<float>();
    AddActorWorldRotation(FRotator(0.0f, RotationInput * RotationSpeed * GetWorld()->GetDeltaSeconds(), 0.0f));
}

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
    if (bPaused) return;
    if (const APlayerController* PlayerController = Cast<APlayerController>(GetController()))
    {
        if (IsMouseOverUI(PlayerController)) return;

        FHitResult HitResult;
        if (!PlayerController->GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel1),
                                                                true, HitResult))
            return;

        if (const ADefenderSpot* Spot = Cast<ADefenderSpot>(HitResult.GetActor()))
        {
            CurrentlySelectedDefenderSpot = Spot;
            CurrentFocusTarget = Spot;
            bFollowTarget = false;
        }

        if (const ADefender* Defender = Cast<ADefender>(HitResult.GetActor()))
        {
            CurrentlySelectedDefenderSpot = Defender->GetSpawnPoint();
            CurrentFocusTarget = Defender;
        }
    }
}

void ATowerDefencePlayer::DoDeselect()
{
    if (bPaused) return;
    CurrentlySelectedDefenderSpot = nullptr;
    CurrentFocusTarget = nullptr;
    bFollowTarget = false;
}

// ReSharper disable once CppMemberFunctionMayBeConst
void ATowerDefencePlayer::DoZoom(const FInputActionValue& Value)
{
    if (bPaused) return;
    const float ScrollAmount = Value.Get<float>();
    SpringArmComponent->TargetArmLength = FMath::Clamp(SpringArmComponent->TargetArmLength - ScrollAmount * ZoomSpeed,
                                                       ClosestCameraDistance, FurthestCameraDistance);
}

void ATowerDefencePlayer::DoFocus()
{
    if (bPaused || !IsValid(CurrentFocusTarget)) return;
    FVector DesiredLocation = CurrentFocusTarget->GetActorLocation();
    DesiredLocation.Z = GetActorLocation().Z;
    SetActorLocation(DesiredLocation);
    SpringArmComponent->TargetArmLength = ClosestCameraDistance;
    bFollowTarget = CurrentFocusTarget->IsA<ADefender>();
}

void ATowerDefencePlayer::DoReset()
{
    if (bPaused) return;
    SetActorLocationAndRotation(FVector {}, FQuat {}, false, nullptr);
    SpringArmComponent->TargetArmLength = FurthestCameraDistance;
}

// ReSharper disable once CppMemberFunctionMayBeConst
void ATowerDefencePlayer::DoPause()
{
    if (ATowerDefencePlayerController* TDPC = Cast<ATowerDefencePlayerController>(GetController())) TDPC->TogglePause();
}
