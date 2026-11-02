#include "TowerDefencePawns/Defenders/Ballista/Ballista.h"

#include "Components/ChildActorComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AIPerceptionTypes.h"
#include "TDCollisionChannels.h"
#include "TowerDefencePawns/AI/ProximityPerception/AISenseConfig_Proximity.h"
#include "TowerDefencePawns/AI/TargetSelectionFunctions.h"
#include "TowerDefencePawns/Components/DamageComponent.h"
#include "TowerDefencePawns/Components/HealthComponent.h"
#include "TowerDefencePawns/Components/HitFlashComponent.h"
#include "TowerDefencePawns/Defenders/Ballista/BallistaBarrel.h"
#include "TowerDefencePawns/Defenders/Ballista/BallistaDecoration.h"
#include "TowerDefencePawns/Defenders/Components/DefenderUpgradeComponent.h"
#include "TowerDefencePawns/Projectile.h"
#include "TowerDefencePawns/ProjectilePoolFactory.h"

ABallista::ABallista()
{
    PawnDisplayName = TEXT("Ballista");
    OccupiedRadius = 200.0f;
    StandMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Stand Mesh"));
    StandMesh->SetupAttachment(RootComponent);
    PivotPoint = CreateDefaultSubobject<USceneComponent>(TEXT("Pivot Point"));
    PivotPoint->SetupAttachment(RootComponent); // Don't attach to stand so doesn't affect scale if resize stand
    PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception Component"));
    ProximityConfig = CreateDefaultSubobject<UAISenseConfig_Proximity>(TEXT("Proximity Config"));
    ProximityConfig->DetectionRadius = AttackRadius;
    ProximityConfig->DetectionByAffiliation.bDetectEnemies = true;
    ProximityConfig->DetectionByAffiliation.bDetectFriendlies = false;
    ProximityConfig->DetectionByAffiliation.bDetectNeutrals = false;
    PerceptionComponent->ConfigureSense(*ProximityConfig);
    PerceptionComponent->SetDominantSense(ProximityConfig->GetSenseImplementation());
    PerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &ABallista::OnTargetPerceptionUpdated);
    BallistaBarrel = CreateDefaultSubobject<UChildActorComponent>(TEXT("Ballista Barrel"));
    BallistaBarrel->SetupAttachment(PivotPoint);
    CannonBarrel = CreateDefaultSubobject<UChildActorComponent>(TEXT("Cannon Barrel"));
    CannonBarrel->SetupAttachment(PivotPoint);
    CurrentTeam = EAITeam::RangedDefender;
    PrimaryRadiusColour = FLinearColor(1.0f, 0.0f, 0.0f, 1.0f);
}

void ABallista::BeginPlay()
{
    Super::BeginPlay();
    if (PivotPoint) DefaultPivotRotation = PivotPoint->GetRelativeRotation();
    HitFlashComponent->BindMaterials();
    if (ABallistaBarrel* Ballista = GetBallistaBarrel(); IsValid(Ballista))
    {
        Ballista->GetHitFlashComponent()->BindMaterials();
        Ballista->GetMesh()->SetCollisionResponseToChannel(MouseClickTraceChannel, ECR_Ignore);
    }
    if (ABallistaBarrel* Cannon = GetCannonBarrel(); IsValid(Cannon))
    {
        Cannon->GetHitFlashComponent()->BindMaterials();
        Cannon->GetMesh()->SetCollisionResponseToChannel(MouseClickTraceChannel, ECR_Ignore);
    }
    StandMesh->SetCollisionResponseToChannel(MouseClickTraceChannel, ECR_Ignore);
    for (auto& Ref : BallistaFirstUpgradeDecorations)
    {
        const UChildActorComponent* Comp = Cast<UChildActorComponent>(Ref.GetComponent(this));
        if (!IsValid(Comp)) continue;
        ABallistaDecoration* Deco = Cast<ABallistaDecoration>(Comp->GetChildActor());
        if (!IsValid(Deco)) continue;
        Deco->GetHitFlashComponent()->BindMaterials();
        Deco->GetMesh()->SetCollisionResponseToChannel(MouseClickTraceChannel, ECR_Ignore);
    }
}

void ABallista::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    if (!IsPawnActiveAndAlive(this)) return;
    TimeSinceLastTargetUpdate += DeltaTime;
    if (TimeSinceLastTargetUpdate >= 1.0f / TargetUpdateFrequency)
    {
        TimeSinceLastTargetUpdate = 0.0f;
        UpdateAttackTarget();
    }
    UpdatePivotRotation(DeltaTime);
    StartAttack();
}

void ABallista::StartAttack()
{
    if (!IsPawnActiveAndAlive(CurrentAttackTarget) || !bCanAttack) return;
    bCanAttack = false;
    FireProjectile();
    GetWorldTimerManager().SetTimer(AttackTimerHandle, [this]() -> void { bCanAttack = true; }, AttackCooldown, false);
}

void ABallista::DoFlash()
{
    Super::DoFlash();
    if (ABallistaBarrel* CurrentBarrel = GetBarrel()) CurrentBarrel->GetHitFlashComponent()->DoFlash();
    if (GetUpgradeComponent()->GetCurrentLevel() != 1) return;
    for (auto& Ref : BallistaFirstUpgradeDecorations)
    {
        const UChildActorComponent* Comp = Cast<UChildActorComponent>(Ref.GetComponent(this));
        if (!IsValid(Comp)) continue;
        if (ABallistaDecoration* Deco = Cast<ABallistaDecoration>(Comp->GetChildActor()); IsValid(Deco))
            Deco->GetHitFlashComponent()->BindMaterials();
    }
}

FBallistaLevel ABallista::GetBallistaCurrentLevel() const
{
    const int32 CurrentLevel = GetUpgradeComponent()->GetCurrentLevel();
    if (CurrentLevel == 0) return BallistaBaseLevel;
    if (CurrentLevel == 1) return BallistaFirstUpgrade;
    return BallistaSecondUpgrade;
}

ABallistaBarrel* ABallista::GetBarrel() const
{
    if (GetUpgradeComponent()->GetCurrentLevel() == 2) return GetCannonBarrel();
    return GetBallistaBarrel();
}

void ABallista::DoOnSetActive(bool bActive)
{
    Super::DoOnSetActive(bActive);
    CurrentAttackTarget = nullptr;
    GetWorldTimerManager().ClearTimer(AttackTimerHandle);
    if (bActive)
    {
        PerceptionComponent->SetActive(true);
        if (UAIPerceptionSystem* PerceptionSys = UAIPerceptionSystem::GetCurrent(GetWorld()))
            PerceptionSys->UpdateListener(*PerceptionComponent);
        GetUpgradeComponent()->SetLevel(0); // Reset to base level
    }
    else
    {
        PerceptionComponent->ForgetAll();
        PerceptionComponent->SetActive(false);
        if (ABallistaBarrel* Ballista = GetBallistaBarrel(); IsValid(Ballista))
        {
            Ballista->GetMesh()->SetVisibility(false);
            Ballista->GetMesh()->SetComponentTickEnabled(false);
            Ballista->GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
            Ballista->GetMesh()->SetCollisionResponseToAllChannels(ECR_Ignore);
        }
        if (ABallistaBarrel* Cannon = GetCannonBarrel(); IsValid(Cannon))
        {
            Cannon->GetMesh()->SetVisibility(false);
            Cannon->GetMesh()->SetComponentTickEnabled(false);
            Cannon->GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
            Cannon->GetMesh()->SetCollisionResponseToAllChannels(ECR_Ignore);
        }
        for (auto& Ref : BallistaFirstUpgradeDecorations)
        {
            const UChildActorComponent* Comp = Cast<UChildActorComponent>(Ref.GetComponent(this));
            if (!IsValid(Comp)) continue;
            ABallistaDecoration* Deco = Cast<ABallistaDecoration>(Comp->GetChildActor());
            if (!IsValid(Deco)) continue;
            Deco->GetMesh()->SetVisibility(false);
            Deco->GetMesh()->SetComponentTickEnabled(false);
            Deco->GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
            Deco->GetMesh()->SetCollisionResponseToAllChannels(ECR_Ignore);
        }
    }
    VisiblePawns.Empty();
    TimeSinceLastTargetUpdate = 0.0f;
    StandMesh->SetVisibility(bActive);
    StandMesh->SetComponentTickEnabled(bActive);
    StandMesh->SetCollisionEnabled(bActive ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision);
    StandMesh->SetCollisionResponseToAllChannels(bActive ? ECR_Block : ECR_Ignore);
    if (bActive)
    {
        StandMesh->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Ignore);
        StandMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    }
    bCanAttack = bActive;
}

void ABallista::DoUpdatePerceptionOnTeamChange()
{
    if (UAIPerceptionSystem* PerceptionSys = UAIPerceptionSystem::GetCurrent(GetWorld()))
        PerceptionSys->UpdateListener(*PerceptionComponent);
    PerceptionComponent->ForgetAll();
    VisiblePawns.Empty();
    CurrentAttackTarget = nullptr;
}

void ABallista::DoOnLevelChanged(int32 Level)
{
    if (ABallistaBarrel* Ballista = GetBallistaBarrel(); IsValid(Ballista))
    {
        Ballista->GetMesh()->SetVisibility(false);
        Ballista->GetMesh()->SetComponentTickEnabled(false);
        Ballista->GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        Ballista->GetMesh()->SetCollisionResponseToAllChannels(ECR_Ignore);
    }
    if (ABallistaBarrel* Cannon = GetCannonBarrel(); IsValid(Cannon))
    {
        Cannon->GetMesh()->SetVisibility(false);
        Cannon->GetMesh()->SetComponentTickEnabled(false);
        Cannon->GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        Cannon->GetMesh()->SetCollisionResponseToAllChannels(ECR_Ignore);
    }
    for (auto& Ref : BallistaFirstUpgradeDecorations)
    {
        const UChildActorComponent* Comp = Cast<UChildActorComponent>(Ref.GetComponent(this));
        if (!IsValid(Comp)) continue;
        ABallistaDecoration* Deco = Cast<ABallistaDecoration>(Comp->GetChildActor());
        if (!IsValid(Deco)) continue;
        Deco->GetMesh()->SetVisibility(false);
        Deco->GetMesh()->SetComponentTickEnabled(false);
        Deco->GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        Deco->GetMesh()->SetCollisionResponseToAllChannels(ECR_Ignore);
    }
    const FBallistaLevel CurrentBallistaLevel = GetBallistaCurrentLevel();
    SetCurrentTeam(CurrentBallistaLevel.GetTeam());
    AttackRadius = CurrentBallistaLevel.GetAttackRadius();
    AttackCooldown = CurrentBallistaLevel.GetAttackCooldown();
    if (ABallistaBarrel* Barrel = GetBallistaBarrel(); IsValid(Barrel))
    {
        Barrel->GetMesh()->SetVisibility(true);
        Barrel->GetMesh()->SetComponentTickEnabled(true);
        Barrel->GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
        Barrel->GetMesh()->SetCollisionResponseToAllChannels(ECR_Block);
        Barrel->GetMesh()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Ignore);
        Barrel->GetMesh()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    }
    if (Level != 1) return;
    for (auto& Ref : BallistaFirstUpgradeDecorations)
    {
        const UChildActorComponent* Comp = Cast<UChildActorComponent>(Ref.GetComponent(this));
        if (!IsValid(Comp)) continue;
        ABallistaDecoration* Deco = Cast<ABallistaDecoration>(Comp->GetChildActor());
        if (!IsValid(Deco)) continue;
        Deco->GetMesh()->SetVisibility(true);
        Deco->GetMesh()->SetComponentTickEnabled(true);
        Deco->GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
        Deco->GetMesh()->SetCollisionResponseToAllChannels(ECR_Block);
        Deco->GetMesh()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Ignore);
        Deco->GetMesh()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    }
}

float ABallista::GetAngleToFireProjectile(const UObject* WorldContextObject, FVector LaunchLocation,
                                          FVector TargetLocation, float InitialVelocity, float ProjectileGravityScale)
{
    // This was... painful to do, my goodness
    if (!WorldContextObject || FMath::IsNearlyZero(InitialVelocity)) return 0.0f;
    const float PosG = FMath::Abs(WorldContextObject->GetWorld()->GetGravityZ() * ProjectileGravityScale);
    const float X = FVector::Dist2D(LaunchLocation, TargetLocation);
    const float H = LaunchLocation.Z - TargetLocation.Z;
    if (FMath::IsNearlyZero(H)) // Same height
    {
        // Formula can be found anywhere online, but this helped me understand:
        // https://www.youtube.com/watch?v=3UYjw30h0jU
        //
        //
        //
        // y = y_0 + v_0y * t + 0.5 * g * t ^ 2 || x = v_0x * t
        //
        // y = y_0 + v_0 * sin(theta) * t - 0.5 * (+g) * t ^ 2 || x = v0 * cos(theta) * t
        //
        // y = y_0 + v_0 * sin(theta) * t - 0.5 * (+g) * t ^ 2 || t = x / (v_0 * cos(theta))
        //
        // y becomes 0 because target y = 0 and y_0 becomes 0 because starting height is 0
        //
        // 0 = 0 + v_0 * sin(theta) * x / (v_0 * cos(theta)) - 0.5 * (+g) * (x / (v_0 * cos(theta))) ^ 2
        //
        // 0 = sin(theta) / cos(theta) - 0.5 * (+g) * (x ^ 2) / (v_0 ^ 2 * cos^2(theta))
        //
        // 0 = sin(theta) * cos(theta) - 0.5 * (+g) * x / (v_0 ^ 2)
        //
        // 0 = 0.5 * sin(2 * theta) - 0.5 * (+g) * x / (v_0 ^ 2)
        //
        // 0 = sin(2 * theta) - (+g) * x) / (v_0 ^ 2)
        //
        // sin(2 * theta) = (+g) * x / (v_0 ^ 2)
        //
        // 2 * theta = asin((+g) * x / (v_0 ^ 2))
        //
        // theta = asin((+g) * x / (v_0 ^ 2)) / 2
        float Theta = FMath::Asin(PosG * X / FMath::Square(InitialVelocity)) / 2.0f;
        return FMath::RadiansToDegrees(Theta);
    }
    // Formula can be found anywhere online, but this helped me understand:
    // https://www.youtube.com/watch?v=bqYtNrhdDAY
    //
    //
    //
    // y = y_0 + v_0y * t + 0.5 * g * t ^ 2 || x = v_0x * t
    //
    // y = y_0 + v_0 * sin(theta) * t - 0.5 * (+g) * t ^ 2 || x = v0 * cos(theta) * t
    //
    // y = y_0 + v_0 * sin(theta) * t - 0.5 * (+g) * t ^ 2 || t = x / (v_0 * cos(theta))
    //
    // y becomes 0 because target y = 0, h = difference in heights
    //
    // 0 = h + v_0 * sin(theta) * (x / (v_0 * cos(theta))) - 0.5 * (+g) * (x / (v_0 * cos(theta))) ^ 2
    //
    // 0 = h + (x * sin(theta)) / cos(theta) - 0.5 * (+g) * (x ^ 2) / (v_0 ^ 2 * cos^2(theta))
    //
    // 0 = h * cos^2(theta) + x * sin(theta) * cos(theta) - 0.5 * (+g) * (x ^ 2) / (v_0 ^ 2)
    //
    // 0 = h * (1 - sin^2(theta)) + x * sin(theta) * cos(theta) - 0.5 * (+g) * (x ^ 2) / (v_0 ^ 2)
    //
    // 0 = h - h * sin^2(theta) + x * sin(theta) * cos(theta) - 0.5 * (+g) * (x ^ 2) / (v_0 ^ 2)
    //
    // 0 = -h * sin^2(theta) + x * sin(theta) * cos(theta) + h - 0.5 * (+g) * (x ^ 2) / (v_0 ^ 2)
    //
    // 0 = -h / 2 * (1 - cos(2 * theta)) + x * 0.5 * sin(2 * theta) + h - 0.5 * (+g) * (x ^ 2) / (v_0 ^ 2)
    //
    // 0 = h / 2 * cos(2 * theta) + x * 0.5 * sin(2 * theta) + h / 2 - 0.5 * (+g) * (x ^ 2) / (v_0 ^ 2)
    //
    // 0 = h * cos(2 * theta) + x * sin(2 * theta) + h - (+g) * (x ^ 2) / (v_0 ^ 2)
    //
    // 0 = sqrt(x ^ 2 + h ^ 2) * cos(2 * theta - atan(x / h)) + h - (+g) * (x ^ 2) / (v_0 ^ 2)
    //
    // -cos(2 * theta - atan(x / h)) = (h - (+g) * (x ^ 2) / (v_0 ^ 2)) / sqrt(x ^ 2 + h ^ 2)
    //
    // cos(2 * theta - atan(x / h)) = ((+g) * (x ^ 2) / (v_0 ^ 2) - h) / sqrt(x ^ 2 + h ^ 2)
    //
    // 2 * theta - atan(x / h) = acos(((+g) * (x ^ 2) / (v_0 ^ 2) - h) / sqrt(x ^ 2 + h ^ 2))
    //
    // 2 * theta = acos(((+g) * (x ^ 2) / (v_0 ^ 2) - h) / sqrt(x ^ 2 + h ^ 2)) + atan(x / h)
    //
    // theta = acos(((+g) * (x ^ 2) / (v_0 ^ 2) - h) / sqrt(x ^ 2 + h ^ 2)) / 2 + atan(x / h) / 2
    // ^ acos has two valid solutions (+ and -) apparently, + is the high lobbing arc, - is the low arc shot, which is
    // what we actually want here, so we use - instead
    const float Theta =
        FMath::Atan(X / H) / 2.0f - FMath::Acos((PosG * FMath::Square(X) / FMath::Square(InitialVelocity) - H) /
                                                FMath::Sqrt(FMath::Square(X) + FMath::Square(H))) /
                                        2.0f;
    return Theta;
}

void ABallista::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
    if (!IsValid(Actor) || !IsPawnActive() || HealthComponent->IsDead()) return;
    if (ATowerDefencePawn* TDPawn = Cast<ATowerDefencePawn>(Actor))
    {
        if (!Stimulus.WasSuccessfullySensed()) VisiblePawns.Remove(TDPawn);
        else if (TDPawn->IsPawnActive() && TDPawn->GetHealthComponent()->IsAlive()) VisiblePawns.AddUnique(TDPawn);
    }
}

void ABallista::UpdateAttackTarget()
{
    if (const ATowerDefencePawn* Current = CurrentAttackTarget;
        IsPawnActiveAndAlive(Current) &&
        FVector::Dist2D(GetActorLocation(), Current->GetActorLocation()) - Current->GetOccupiedRadius() <=
            AttackRadius + KINDA_SMALL_NUMBER)
        return;
    if (ATowerDefencePawn* NewTarget = SelectClosestTarget(VisiblePawns, this)) SetAttackTarget(NewTarget);
    else CurrentAttackTarget = nullptr;
}

void ABallista::UpdatePivotRotation(float DeltaTime)
{
    if (!PivotPoint) return;
    FRotator TargetRelativeRotation = DefaultPivotRotation;
    if (IsValid(CurrentAttackTarget))
    {
        FVector Direction = CurrentAttackTarget->GetActorLocation() - PivotPoint->GetComponentLocation();
        Direction.Z = 0.0f;
        if (!Direction.IsNearlyZero())
        {
            const USceneComponent* Parent = PivotPoint->GetAttachParent();
            const FVector LocalDirection =
                Parent ? Parent->GetComponentTransform().InverseTransformVectorNoScale(Direction) : Direction;
            TargetRelativeRotation = FRotator(0.0f, LocalDirection.Rotation().Yaw, 0.0f);
        }
    }
    const FRotator NewRotation =
        FMath::RInterpConstantTo(PivotPoint->GetRelativeRotation(), TargetRelativeRotation, DeltaTime, RotationSpeed);
    PivotPoint->SetRelativeRotation(NewRotation);
}

void ABallista::FireProjectile()
{
    const FBallistaLevel Current = GetBallistaCurrentLevel();
    const auto ProjectileClass = Current.GetProjectile().GetProjectileClass();
    const ABallistaBarrel* Barrel = GetBarrel();
    if (!IsValid(Barrel)) return;
    const USceneComponent* Muzzle = Barrel->GetMuzzle();
    if (!Muzzle) return;
    if (!IsValid(CurrentAttackTarget) || !ProjectileClass) return;
    if (PROJECTILE_POOL_FACTORY_EXISTS)
    {
        const FVector LaunchLocation = Muzzle->GetComponentLocation();
        const auto Projectile = CREATE_PROJECTILE(ProjectileClass, FTransform(LaunchLocation));
        if (!Projectile) return;
        const FVector TargetLocation = CurrentAttackTarget->GetVisualAttackPointLocation();
        const FVector LaunchVelocity = ComputeLaunchVelocity(LaunchLocation, TargetLocation);
        if (!GetBallistaCurrentLevel().DamageGroundTroops())
            Projectile->Fire(CurrentAttackTarget->GetVisualAttackPoint(), LaunchVelocity, DamageComponent->GetDamage(),
                             false);
        else if (GetUpgradeComponent()->GetCurrentLevel() == 1)
            Projectile->Fire(CurrentAttackTarget->GetVisualAttackPoint(), LaunchVelocity, DamageComponent->GetDamage(),
                             true, GroundTroopDamageReduction);
        else
            Projectile->Fire(CurrentAttackTarget->GetVisualAttackPoint(), LaunchVelocity, DamageComponent->GetDamage(),
                             GroundTroopDamageReduction, ExplosionRadius);
    }
}

FVector ABallista::ComputeLaunchVelocity(const FVector& LaunchLocation, const FVector& TargetLocation) const
{
    const FBallistaLevel Current = GetBallistaCurrentLevel();
    const float ProjectileSpeed = Current.GetProjectile().GetProjectileSpeed();
    FVector HorizontalDirection = TargetLocation - LaunchLocation;
    HorizontalDirection.Z = 0.0f;
    if (!HorizontalDirection.Normalize()) return FVector::ZeroVector;
    const float ThetaRadians =
        GetAngleToFireProjectile(this, LaunchLocation, TargetLocation, ProjectileSpeed, ProjectileGravityScale);
    return (HorizontalDirection * FMath::Cos(ThetaRadians) + FVector::UpVector * FMath::Sin(ThetaRadians)) *
           ProjectileSpeed;
}

ABallistaBarrel* ABallista::GetBallistaBarrel() const
{
    return Cast<ABallistaBarrel>(BallistaBarrel ? BallistaBarrel->GetChildActor() : nullptr);
}

ABallistaBarrel* ABallista::GetCannonBarrel() const
{
    return Cast<ABallistaBarrel>(CannonBarrel ? CannonBarrel->GetChildActor() : nullptr);
}
