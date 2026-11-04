#include "TowerDefencePawns/Defenders/Defender.h"

#include "Components/BoxComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "NiagaraComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "TowerDefenceGameMode.h"
#include "TowerDefencePawns/AI/ProximityPerception/AISense_Proximity.h"
#include "TowerDefencePawns/AI/ProximityPerception/AISenseConfig_Proximity.h"
#include "TowerDefencePawns/Components/DamageComponent.h"
#include "TowerDefencePawns/Components/HealthComponent.h"
#include "TowerDefencePawns/Components/OutlineComponent.h"
#include "TowerDefencePawns/Defenders/AI/DefenderAIController.h"
#include "TowerDefencePawns/Defenders/Components/DefenderUpgradeComponent.h"

ADefender::ADefender()
{
    PawnDisplayName = TEXT("PlayerTroop");
    BoxCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Collider"));
    BoxCollider->SetupAttachment(RootComponent);
    BoxCollider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    BoxCollider->SetCollisionObjectType(ECC_WorldDynamic);
    BoxCollider->SetCollisionResponseToAllChannels(ECR_Ignore);
    BoxCollider->SetCollisionResponseToChannel(MouseClickTraceChannel, ECR_Block);
    HealEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Heal Effect"));
    HealEffect->SetupAttachment(RootComponent);
    HealEffect->bAutoActivate = false;
    DefenderRadiusDisplay = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Defender Radius Display"));
    DefenderRadiusDisplay->SetupAttachment(RootComponent);
    DefenderRadiusDisplay->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    DefenderRadiusDisplay->SetCastShadow(false);
    DefenderRadiusDisplay->SetVisibility(false);
    OutlineComponent = CreateDefaultSubobject<UOutlineComponent>(TEXT("Outline Component"));
    UpgradeComponent = CreateDefaultSubobject<UDefenderUpgradeComponent>(TEXT("Upgrade Component"));
}

void ADefender::PostInitializeComponents()
{
    Super::PostInitializeComponents();
    UpgradeComponent->OnLevelChanged.AddUniqueDynamic(this, &ADefender::LevelChanged);
    HealthComponent->OnHealthLostValue.AddUniqueDynamic(this, &ADefender::OnHealthLostValue);
    HealthComponent->OnHealthGainedValue.AddUniqueDynamic(this, &ADefender::OnHealthGainedValue);
}

void ADefender::BeginPlay()
{
    Super::BeginPlay();
    DefenderRadiusMaterial = DefenderRadiusDisplay->CreateAndSetMaterialInstanceDynamic(0);
}

void ADefender::ShowRadiusDisplay()
{
    if (!DefenderRadiusMaterial) return;
    bShowingRadius = true;
    const auto Pack = [](const FLinearColor& Colour, float Radius) -> FLinearColor
    { return FLinearColor(Colour.R, Colour.G, Colour.B, Radius); };
    DefenderRadiusMaterial->SetVectorParameterValue(TEXT("Detection Radius"),
                                                    Pack(DetectionRadiusColour, GetDetectionRadius()));
    DefenderRadiusMaterial->SetVectorParameterValue(TEXT("Primary Radius"),
                                                    Pack(PrimaryRadiusColour, GetPrimaryRadius()));
    DefenderRadiusMaterial->SetVectorParameterValue(TEXT("Secondary Radius"),
                                                    Pack(SecondaryRadiusColour, GetSecondaryRadius()));
    DefenderRadiusDisplay->SetVisibility(true);
}

void ADefender::ShowOutline()
{
    OutlineComponent->EnableOutline();
    bShowingOutline = true;
}

void ADefender::HideRadiusDisplay()
{
    DefenderRadiusDisplay->SetVisibility(false);
    bShowingRadius = false;
}

void ADefender::HideOutline()
{
    OutlineComponent->DisableOutline();
    bShowingOutline = false;
}

float ADefender::GetDetectionRadius() const
{
    const ATowerDefencePawnAIController* AIC = GetController<ATowerDefencePawnAIController>();
    if (!AIC) return 0.0f;
    if (const UAIPerceptionComponent* Perception = AIC->GetAIPerceptionComponent())
        if (const auto* Config =
                Cast<UAISenseConfig_Proximity>(Perception->GetSenseConfig(UAISense::GetSenseID<UAISense_Proximity>())))
            return Config->DetectionRadius;
    if (const UAISenseConfig_Proximity* Config = AIC->GetProximityConfig()) return Config->DetectionRadius;
    return 0.0f;
}

TOptional<FName> ADefender::GetDefenderName(TSubclassOf<ADefender> DefenderClass, int32 Level)
{
    if (Level < 0 || Level > 2 || !DefenderClass) return {};
    const ADefender* DefaultDefender = DefenderClass->GetDefaultObject<ADefender>();
    if (!DefaultDefender) return {};
    if (Level == 0) return DefaultDefender->GetUpgradeComponent()->GetBaseLevel().GetName();
    if (Level == 1) return DefaultDefender->GetUpgradeComponent()->GetFirstUpgrade().GetName();
    return DefaultDefender->GetUpgradeComponent()->GetSecondUpgrade().GetName();
}

TOptional<int32> ADefender::GetDefenderCost(TSubclassOf<ADefender> DefenderClass, int32 Level)
{
    if (Level < 0 || Level > 2 || !DefenderClass) return {};
    const ADefender* DefaultDefender = DefenderClass->GetDefaultObject<ADefender>();
    if (!DefaultDefender) return {};
    if (Level == 0) return DefaultDefender->GetUpgradeComponent()->GetBaseLevel().GetCost();
    if (Level == 1) return DefaultDefender->GetUpgradeComponent()->GetFirstUpgrade().GetCost();
    return DefaultDefender->GetUpgradeComponent()->GetSecondUpgrade().GetCost();
}

int32 ADefender::GetSellPrice() const
{
    const FDefenderLevel Current = UpgradeComponent->GetCurrent();
    return Current.GetSellPrice();
}

void ADefender::DoOnSetActive(bool bActive)
{
    BoxCollider->SetCollisionEnabled(bActive ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
    HideRadiusDisplay();
}

void ADefender::LevelChanged(int32 Level)
{
    if (bUseAIController)
        if (ADefenderAIController* D_AIC = GetController<ADefenderAIController>()) D_AIC->UpdateControllerLevel();
    const FDefenderLevel CurrentDefenderLevel = GetUpgradeComponent()->GetCurrent();
    DamageComponent->SetDamage(CurrentDefenderLevel.GetDamage());
    HealthComponent->SetMaxHealth(CurrentDefenderLevel.GetHealth());
    DoOnLevelChanged(Level);
}

void ADefender::OnHealthLostValue(int32 Amount)
{
    if (TowerDefenceGameMode) TowerDefenceGameMode->DefenderLostHealth(Amount);
}

void ADefender::OnHealthGainedValue(int32 Amount)
{
    if (TowerDefenceGameMode) TowerDefenceGameMode->DefenderGainedHealth(Amount);
}
