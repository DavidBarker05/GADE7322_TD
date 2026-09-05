// ReSharper disable CppParameterMayBeConst
#include "Player/TowerDefence/TowerDefencePlayer.h"

#include "Camera/CameraComponent.h"
#include "CustomLog.h"
#include "EnhancedInputComponent.h"
#include "Framework/Application/SlateApplication.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputActionValue.h"
#include "Player/TowerDefence/Components/CurrencyComponent.h"
#include "Player/TowerDefence/TowerDefencePlayerController.h"
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
    CurrencyComponent->ResetCurrency();
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

void ATowerDefencePlayer::SwitchBetweenSpotAndDefender()
{
    if (!IsValid(CurrentFocusTarget)) return;
    if (ADefenderSpot* Spot = Cast<ADefenderSpot>(CurrentFocusTarget))
    {
        if (!IsValid(Spot->GetCurrentDefender())) return;
        CurrentFocusTarget = Spot->GetCurrentDefender();
    }
    else if (ADefender* Defender = Cast<ADefender>(CurrentFocusTarget))
    {
        if (!IsValid(Defender->GetSpawnPoint())) return;
        CurrentFocusTarget = Defender->GetSpawnPoint();
    }
}

// ReSharper disable once CppMemberFunctionMayBeConst
void ATowerDefencePlayer::DoMove(const FInputActionValue& Value)
{
    if (IsGamePaused()) return;
    const FVector2D MovementVector = Value.Get<FVector2D>();
    const FVector RightVector = GetActorRightVector() * MovementVector.X;
    const FVector ForwardVector = GetActorForwardVector() * MovementVector.Y;
    FloatingPawnMovement->AddInputVector(RightVector + ForwardVector);
    if (MovementVector.SquaredLength() > KINDA_SMALL_NUMBER) bFollowTarget = false;
}

// ReSharper disable once CppMemberFunctionMayBeConst
void ATowerDefencePlayer::DoRotate(const FInputActionValue& Value)
{
    if (IsGamePaused()) return;
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

bool ATowerDefencePlayer::IsGamePaused() const
{
    if (const APlayerController* PC = GetController<APlayerController>()) return PC->IsPaused();
    return false;
}

// ReSharper disable once CppMemberFunctionMayBeConst
void ATowerDefencePlayer::DoSelect()
{
    if (IsGamePaused()) return;
    if (const APlayerController* PlayerController = GetController<APlayerController>())
    {
        if (IsMouseOverUI(PlayerController)) return;

        FHitResult HitResult;
        if (!PlayerController->GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel1),
                                                                true, HitResult))
            return;

        if (ADefenderSpot* Spot = Cast<ADefenderSpot>(HitResult.GetActor()))
        {
            CurrentlySelectedDefenderSpot = Spot;
            CurrentFocusTarget = Spot;
            bFollowTarget = false;
            BROADCAST_EVENT(TEXT("UpdateHUDEvent"), FName(TEXT("PawnManager")), FName(TEXT("Select")), Spot);
        }
        else if (ADefender* Defender = Cast<ADefender>(HitResult.GetActor()))
        {
            CurrentlySelectedDefenderSpot = Defender->GetSpawnPoint();
            CurrentFocusTarget = Defender;
            BROADCAST_EVENT(TEXT("UpdateHUDEvent"), FName(TEXT("PawnManager")), FName(TEXT("Select")), Defender);
        }
        else if (APlayerTower* Tower = Cast<APlayerTower>(HitResult.GetActor()))
        {
            CurrentlySelectedDefenderSpot = nullptr;
            CurrentFocusTarget = Tower;
            bFollowTarget = false;
            BROADCAST_EVENT(TEXT("UpdateHUDEvent"), FName(TEXT("PawnManager")), FName(TEXT("Select")), Tower);
        }
    }
}

void ATowerDefencePlayer::DoDeselect()
{
    if (IsGamePaused()) return;
    CurrentlySelectedDefenderSpot = nullptr;
    CurrentFocusTarget = nullptr;
    bFollowTarget = false;
    BROADCAST_EVENT(TEXT("UpdateHUDEvent"), FName(TEXT("PawnManager")), FName(TEXT("Deselect")));
}

// ReSharper disable once CppMemberFunctionMayBeConst
void ATowerDefencePlayer::DoZoom(const FInputActionValue& Value)
{
    if (IsGamePaused()) return;
    const float ScrollAmount = Value.Get<float>();
    SpringArmComponent->TargetArmLength = FMath::Clamp(SpringArmComponent->TargetArmLength - ScrollAmount * ZoomSpeed,
                                                       ClosestCameraDistance, FurthestCameraDistance);
}

void ATowerDefencePlayer::DoFocus()
{
    if (IsGamePaused() || !IsValid(CurrentFocusTarget)) return;
    FVector DesiredLocation = CurrentFocusTarget->GetActorLocation();
    DesiredLocation.Z = GetActorLocation().Z;
    SetActorLocation(DesiredLocation);
    SpringArmComponent->TargetArmLength = ClosestCameraDistance;
    bFollowTarget = CurrentFocusTarget->IsA<ADefender>();
}

void ATowerDefencePlayer::DoReset()
{
    if (IsGamePaused()) return;
    SetActorLocationAndRotation(FVector {}, FQuat {}, false, nullptr);
    SpringArmComponent->TargetArmLength = FurthestCameraDistance;
}

// ReSharper disable once CppMemberFunctionMayBeConst
void ATowerDefencePlayer::DoPause()
{
    if (ATowerDefencePlayerController* TD_PC = GetController<ATowerDefencePlayerController>()) TD_PC->TogglePause();
}
