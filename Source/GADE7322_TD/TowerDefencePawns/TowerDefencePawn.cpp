// ReSharper disable CppParameterMayBeConst
#include "TowerDefencePawns/TowerDefencePawn.h"

#include "Components/CapsuleComponent.h"
#include "Components/ProgressBar.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "TowerDefencePawns/AI/ProximityPerception/AISense_Proximity.h"
#include "TowerDefencePawns/AI/TowerDefencePawnAIController.h"
#include "TowerDefencePawns/Components/DamageComponent.h"
#include "TowerDefencePawns/Components/HealthComponent.h"
#include "TowerDefencePawns/Components/HitFlashComponent.h"
#include "UI/TowerDefence/Widgets/HealthBarWidget.h"

ATowerDefencePawn::ATowerDefencePawn()
{
    PrimaryActorTick.bCanEverTick = true;
    GetCapsuleComponent()->SetCollisionProfileName(TEXT("Pawn"));
    GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    GetCharacterMovement()->bEnablePhysicsInteraction = false;
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
    bUseControllerRotationYaw = false;
    GetCharacterMovement()->AirControl = 1.0f;
    GetCharacterMovement()->MaxStepHeight = 70.0f;
    GetCharacterMovement()->SetWalkableFloorAngle(60.0f);
    HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("Health Component"));
    DamageComponent = CreateDefaultSubobject<UDamageComponent>(TEXT("Damage Component"));
    StimuliSourceComponent =
        CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("Stimuli Source Component"));
    HealthBar = CreateDefaultSubobject<UWidgetComponent>(TEXT("Health Bar"));
    HealthBar->SetupAttachment(RootComponent);
    HitFlashComponent = CreateDefaultSubobject<UHitFlashComponent>(TEXT("Hit Flash Component"));
    BaseEyeHeight = 0.0f;
}

void ATowerDefencePawn::BeginPlay()
{
    Super::BeginPlay();
    if (!bIsPawnActive)
    {
        StimuliSourceComponent->UnregisterFromPerceptionSystem();
        if (bUseAIController)
            if (ATowerDefencePawnAIController* AIController = Cast<ATowerDefencePawnAIController>(GetController()))
                AIController->SetControllerActive(false);
    }
}

void ATowerDefencePawn::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    if (!bIsHealthBarShowing) return;
    const FVector BarLoc = HealthBar->GetComponentLocation();
    const FVector CamLoc = UGameplayStatics::GetPlayerCameraManager(this, 0)->GetCameraLocation();
    const FRotator DesiredRot = UKismetMathLibrary::FindLookAtRotation(BarLoc, CamLoc);
    HealthBar->SetWorldRotation(DesiredRot);
}

ATowerDefencePawn& ATowerDefencePawn::SetPawnActive(bool bActive)
{
    bIsPawnActive = bActive;
    if (bActive)
    {
        StimuliSourceComponent->RegisterWithPerceptionSystem();
        StimuliSourceComponent->RegisterForSense(UAISense_Proximity::StaticClass());
        HealthComponent->ResetHealth();
        if (UHealthBarWidget* HealthBarWidget = Cast<UHealthBarWidget>(HealthBar->GetUserWidgetObject()))
        {
            const int32 CurrentHealth = HealthComponent->GetCurrentHealth();
            const int32 MaxHealth = HealthComponent->GetMaxHealth();
            const float HealthPercent = static_cast<float>(CurrentHealth) / static_cast<float>(MaxHealth);
            HealthBarWidget->GetHealthBar()->SetPercent(HealthPercent);
        }
        if (bAlwaysDisplayHealth) ShowHealthBar();
        else HideHealthBar();
    }
    else
    {
        StimuliSourceComponent->UnregisterFromPerceptionSystem();
        StimuliSourceComponent->UnregisterFromSense(UAISense_Proximity::StaticClass());
        HideHealthBar();
    }
    if (bUseAIController)
        if (ATowerDefencePawnAIController* AIController = Cast<ATowerDefencePawnAIController>(GetController()))
            AIController->SetControllerActive(bActive);
    DoOnSetActive(bActive);
    return *this;
}

void ATowerDefencePawn::UpdateHealthDisplay()
{
    UHealthBarWidget* HealthBarWidget = Cast<UHealthBarWidget>(HealthBar->GetUserWidgetObject());
    if (!HealthBarWidget) return;
    const int32 CurrentHealth = HealthComponent->GetCurrentHealth();
    if (CurrentHealth == 0) return; // Don't display on death
    const int32 MaxHealth = HealthComponent->GetMaxHealth();
    const float HealthPercent = static_cast<float>(CurrentHealth) / static_cast<float>(MaxHealth);
    HealthBarWidget->GetHealthBar()->SetPercent(HealthPercent);
    if (!bAlwaysDisplayHealth)
    {
        ShowHealthBar();
        GetWorldTimerManager().SetTimer(HealthBarDisplayHandle, this, &ATowerDefencePawn::HideHealthBar,
                                        HealthDisplayTime, false);
    }
}

void ATowerDefencePawn::ShowHealthBar()
{
    bIsHealthBarShowing = true;
    HealthBar->SetVisibility(true);
}

void ATowerDefencePawn::HideHealthBar()
{
    bIsHealthBarShowing = false;
    HealthBar->SetVisibility(false);
}

void ATowerDefencePawn::Attack(ATowerDefencePawn* Other)
{
    if (IsValid(Other)) DamageComponent->DamageOther(Other->HealthComponent);
}

void ATowerDefencePawn::OnDeath(TFunction<void()>&& Func)
{
    if (bDeathStarted) return;
    bDeathStarted = true;
    DestroyDelegate = MoveTemp(Func);
    OnDeathComplete();
}

void ATowerDefencePawn::OnDeathComplete()
{
    if (!DestroyDelegate) return;
    bDeathStarted = false;
    DestroyDelegate();
    DestroyDelegate = nullptr;
}
