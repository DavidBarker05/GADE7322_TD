#include "TowerDefencePawns/Defenders/Defender.h"

#include "Components/BoxComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "NiagaraComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "TDCollisionChannels.h"
#include "TowerDefencePawns/AI/ProximityPerception/AISense_Proximity.h"
#include "TowerDefencePawns/AI/ProximityPerception/AISenseConfig_Proximity.h"
#include "TowerDefencePawns/AI/TowerDefencePawnAIController.h"

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
}

void ADefender::BeginPlay()
{
    Super::BeginPlay();
    DefenderRadiusMaterial = DefenderRadiusDisplay->CreateAndSetMaterialInstanceDynamic(0);
}

void ADefender::ShowRadiusDisplay()
{
    if (!DefenderRadiusMaterial) return;
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

void ADefender::HideRadiusDisplay() { DefenderRadiusDisplay->SetVisibility(false); }

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

void ADefender::DoOnSetActive(bool bActive)
{
    BoxCollider->SetCollisionEnabled(bActive ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
    HideRadiusDisplay();
}
