// ReSharper disable CppParameterMayBeConst
#include "TowerDefencePawns/TowerDefencePawn.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "TowerDefencePawns/AI/ProximityPerception/AISense_Proximity.h"
#include "TowerDefencePawns/AI/TowerDefencePawnAIController.h"
#include "TowerDefencePawns/Components/DamageComponent.h"
#include "TowerDefencePawns/Components/HealthComponent.h"

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
    HealthComponent = CreateDefaultSubobject<UHealthComponent>("Health Component");
    DamageComponent = CreateDefaultSubobject<UDamageComponent>("Damage Component");
    StimuliSourceComponent = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>("Stimuli Source Component");
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

ATowerDefencePawn& ATowerDefencePawn::SetPawnActive(bool bActive)
{
    bIsPawnActive = bActive;
    if (bActive)
    {
        StimuliSourceComponent->RegisterWithPerceptionSystem();
        StimuliSourceComponent->RegisterForSense(UAISense_Proximity::StaticClass());
        HealthComponent->ResetHealth();
    }
    else
    {
        StimuliSourceComponent->UnregisterFromPerceptionSystem();
        StimuliSourceComponent->UnregisterFromSense(UAISense_Proximity::StaticClass());
    }
    if (bUseAIController)
        if (ATowerDefencePawnAIController* AIController = Cast<ATowerDefencePawnAIController>(GetController()))
            AIController->SetControllerActive(bActive);
    DoOnSetActive(bActive);
    return *this;
}

void ATowerDefencePawn::Tick(float DeltaTime) { Super::Tick(DeltaTime); }

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
