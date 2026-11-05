#include "TowerDefencePawns/Tower/PlayerTower.h"

#include "Components/BoxComponent.h"
#include "Components/DecalComponent.h"
#include "Engine/PostProcessVolume.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "NiagaraComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AIPerceptionTypes.h"
#include "TowerDefencePawns/AI/ProximityPerception/AISenseConfig_Proximity.h"
#include "TowerDefencePawns/AI/TargetSelectionFunctions.h"
#include "TowerDefencePawns/Components/HealthComponent.h"
#include "TowerDefencePawns/Components/HitFlashComponent.h"
#include "TowerDefencePawns/Components/OutlineComponent.h"

APlayerTower::APlayerTower()
{
    PawnDisplayName = TEXT("Main Tower");
    OccupiedRadius = 200.0f;
    TowerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Tower Mesh"));
    TowerMesh->SetupAttachment(RootComponent);
    CrystalMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Crystal Mesh"));
    CrystalMesh->SetupAttachment(RootComponent);
    RadiusDisplay = CreateDefaultSubobject<UDecalComponent>(TEXT("Radius Display"));
    RadiusDisplay->SetupAttachment(RootComponent);
    RadiusDisplay->SetVisibility(false);
    OutlineComponent = CreateDefaultSubobject<UOutlineComponent>(TEXT("Outline Component"));
    AttackTargets.Init(nullptr, 3);
    TimerHandles.SetNum(3);
    PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception Component"));
    ProximityConfig = CreateDefaultSubobject<UAISenseConfig_Proximity>(TEXT("Proximity Config"));
    ProximityConfig->DetectionRadius = AttackRadius;
    ProximityConfig->DetectionByAffiliation.bDetectEnemies = true;
    ProximityConfig->DetectionByAffiliation.bDetectFriendlies = false;
    ProximityConfig->DetectionByAffiliation.bDetectNeutrals = false;
    PerceptionComponent->ConfigureSense(*ProximityConfig);
    PerceptionComponent->SetDominantSense(ProximityConfig->GetSenseImplementation());
    PerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &APlayerTower::OnTargetPerceptionUpdated);
    BoxCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Collider"));
    BoxCollider->SetupAttachment(RootComponent);
    BoxCollider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    BoxCollider->SetCollisionObjectType(ECC_WorldDynamic);
    BoxCollider->SetCollisionResponseToAllChannels(ECR_Ignore);
    BoxCollider->SetCollisionResponseToChannel(MouseClickTraceChannel, ECR_Block);
    CurrentTeam = EAITeam::PlayerTower;
    BeamComponents.SetNum(3);
    for (int32 i = 0; i < 3; ++i)
    {
        UNiagaraComponent* Beam =
            CreateDefaultSubobject<UNiagaraComponent>(*FString::Printf(TEXT("Attack Beam %d"), i));
        Beam->SetupAttachment(RootComponent);
        Beam->SetAutoActivate(false);
        BeamComponents[i] = Beam;
    }
}

void APlayerTower::PostInitializeComponents()
{
    Super::PostInitializeComponents();
    HitFlashComponent->OnMeshMaterialsAdded.AddUniqueDynamic(this, &APlayerTower::OnHitFlashMaterialsAdded);
    HealthComponent->OnHealthChanged.AddUniqueDynamic(this, &APlayerTower::UpdateCrystalMaterial);
    HealthComponent->OnHealthLost.AddUniqueDynamic(this, &APlayerTower::StartScreenFlash);
}

void APlayerTower::BeginPlay()
{
    Super::BeginPlay();
    RadiusMaterial = RadiusDisplay->CreateDynamicMaterialInstance();
    SetPawnActive(true);
    ResetHealth();
    HitFlashComponent->BindMaterials();
    UpdateCrystalMaterial();
    OutlineComponent->BindMaterials();
    OutlineComponent->DisableOutline();
    if (ScreenDamageMaterialInterface)
        ScreenDamageMaterial = UMaterialInstanceDynamic::Create(ScreenDamageMaterialInterface, this);
    if (IsValid(ScreenDamageMaterial))
    {
        ScreenDamageMaterial->SetScalarParameterValue("Alpha", 0.0f);
        ScreenDamageMaterial->SetScalarParameterValue(TEXT("Health01"), GetHealth01());
        if (IsValid(PostProcessVolume))
        {
            const FWeightedBlendable BlendWeight {1.0f, ScreenDamageMaterial};
            PostProcessVolume->Settings.WeightedBlendables.Array.Add(BlendWeight);
        }
    }
}

void APlayerTower::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    TimeSinceLastTargetUpdate += DeltaTime;
    if (TimeSinceLastTargetUpdate >= 1.0f / TargetUpdateFrequency)
    {
        TimeSinceLastTargetUpdate = 0.0f;
        UpdateAttackTargets();
    }
    StartAttack();
    if (!IsValid(ScreenDamageMaterial)) return;
    if (CurrentScreenDamageTime <= 0.0f + UE_KINDA_SMALL_NUMBER)
    {
        ScreenDamageMaterial->SetScalarParameterValue("Alpha", 0.0f);
        return;
    }
    const float Alpha = FTDMath::InverseLerp(0.0f, ScreenDamageDuration, CurrentScreenDamageTime);
    ScreenDamageMaterial->SetScalarParameterValue(TEXT("Alpha"), Alpha);
    CurrentScreenDamageTime -= DeltaTime;
}

void APlayerTower::StartAttack()
{
    for (int32 i = 0; i < 3; ++i)
    {
        if (IsPawnActiveAndAlive(AttackTargets[i]) && CanAttackTarget[i])
        {
            CanAttackTarget[i] = false;
            Attack(AttackTargets[i]);
            FireAttackBeam(i, AttackTargets[i]->GetVisualAttackPointLocation());
            GetWorldTimerManager().SetTimer(
                TimerHandles[i], [this, i]() -> void { CanAttackTarget[i] = true; }, AttackCooldown, false);
        }
    }
}

void APlayerTower::ShowRadiusDisplay()
{
    if (!RadiusMaterial) return;
    const auto Pack = [](const FLinearColor& Colour, float Radius) -> FLinearColor
    { return FLinearColor(Colour.R, Colour.G, Colour.B, Radius); };
    RadiusMaterial->SetVectorParameterValue(TEXT("Detection Radius"), Pack(DetectionRadiusColour, AttackRadius));
    RadiusMaterial->SetVectorParameterValue(TEXT("Primary Radius"), Pack(PrimaryRadiusColour, AttackRadius));
    RadiusMaterial->SetVectorParameterValue(TEXT("Secondary Radius"), Pack(SecondaryRadiusColour, 0.0f));
    RadiusDisplay->DecalSize = FVector(AttackRadius, AttackRadius, AttackRadius);
    RadiusDisplay->SetVisibility(true);
}

void APlayerTower::ShowOutline() { OutlineComponent->EnableOutline(); }

void APlayerTower::HideRadiusDisplay() { RadiusDisplay->SetVisibility(false); }

void APlayerTower::HideOutline() { OutlineComponent->DisableOutline(); }

void APlayerTower::DoUpdatePerceptionOnTeamChange()
{
    if (UAIPerceptionSystem* PerceptionSys = UAIPerceptionSystem::GetCurrent(GetWorld()))
        PerceptionSys->UpdateListener(*PerceptionComponent);
    PerceptionComponent->ForgetAll();
    VisiblePawns.Empty();
    AttackTargets.Empty();
}

void APlayerTower::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
    if (!IsValid(Actor)) return;
    if (ATowerDefencePawn* TDPawn = Cast<ATowerDefencePawn>(Actor))
    {
        if (!Stimulus.WasSuccessfullySensed()) VisiblePawns.Remove(TDPawn);
        else if (IsPawnActiveAndAlive(TDPawn)) VisiblePawns.AddUnique(TDPawn);
    }
}

void APlayerTower::UpdateAttackTargets()
{
    for (int32 i = 0; i < 3; ++i)
    {
        if (const ATowerDefencePawn* Current = AttackTargets[i];
            IsPawnActiveAndAlive(Current) &&
            FVector::Dist2D(GetActorLocation(), Current->GetActorLocation()) - Current->GetOccupiedRadius() <=
                AttackRadius + UE_KINDA_SMALL_NUMBER)
            continue;
        if (ATowerDefencePawn* NewTarget = SelectClosestTarget(VisiblePawns, this)) SetAttackTarget(i, NewTarget);
        else AttackTargets[i] = nullptr;
    }
}

void APlayerTower::FireAttackBeam(int32 Index, const FVector& TargetLocation)
{
    UNiagaraComponent* Beam = BeamComponents[Index];
    if (!Beam) return;
    Beam->SetVariablePosition(TEXT("Beam Start"), Beam->GetComponentLocation());
    Beam->SetVariablePosition(TEXT("Beam End"), TargetLocation);
    Beam->Activate(true);
}

void APlayerTower::OnHitFlashMaterialsAdded(UMeshComponent* MeshComponent,
                                            TArray<UMaterialInstanceDynamic*> MaterialInstances)
{
    if (!IsValid(MeshComponent) || MaterialInstances.IsEmpty()) return;
    CrystalMaterials = MaterialInstances;
}

void APlayerTower::UpdateCrystalMaterial()
{
    for (const auto Material : CrystalMaterials)
    {
        if (!IsValid(Material)) continue;
        Material->SetScalarParameterValue(TEXT("Health01"), GetHealth01());
    }
}

void APlayerTower::StartScreenFlash()
{
    if (!IsValid(ScreenDamageMaterial)) return;
    ScreenDamageMaterial->SetScalarParameterValue(TEXT("Health01"), GetHealth01());
    CurrentScreenDamageTime = ScreenDamageDuration;
}

float APlayerTower::GetHealth01() const
{
    return static_cast<float>(HealthComponent->GetCurrentHealth()) /
           static_cast<float>(HealthComponent->GetBaseMaxHealth());
}
