#include "TowerDefencePawns/Defenders/Ballista/Ballista.h"

#include "HealthComponent.h"
#include "HitFlashComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AIPerceptionTypes.h"
#include "TowerDefencePawns/AI/ProximityPerception/AISenseConfig_Proximity.h"
#include "TowerDefencePawns/AI/TargetSelectionFunctions.h"
#include "TowerDefencePawns/Components/DamageComponent.h"
#include "TowerDefencePawns/Defenders/Ballista/BallistaBolt.h"
#include "TowerDefencePawns/ProjectilePoolFactory.h"

ABallista::ABallista()
{
    PawnDisplayName = TEXT("Ballista");
    OccupiedRadius = 200.0f;
    StandMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Stand Mesh"));
    StandMesh->SetupAttachment(RootComponent);
    PivotPoint = CreateDefaultSubobject<USceneComponent>(TEXT("Pivot Point"));
    PivotPoint->SetupAttachment(RootComponent); // Don't attach to stand so doesn't affect scale if resize stand
    BallistaMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Ballista Mesh"));
    BallistaMesh->SetupAttachment(PivotPoint);
    // ^ So that we can pivot around the point rather than the mesh because mesh isn't centred
    MuzzlePoint = CreateDefaultSubobject<USceneComponent>(TEXT("Muzzle Point"));
    MuzzlePoint->SetupAttachment(PivotPoint); // So it turns with the mesh, position at the tip in the Blueprint
    PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception Component"));
    ProximityConfig = CreateDefaultSubobject<UAISenseConfig_Proximity>(TEXT("Proximity Config"));
    ProximityConfig->DetectionRadius = AttackRadius;
    ProximityConfig->DetectionByAffiliation.bDetectEnemies = true;
    ProximityConfig->DetectionByAffiliation.bDetectFriendlies = false;
    ProximityConfig->DetectionByAffiliation.bDetectNeutrals = false;
    PerceptionComponent->ConfigureSense(*ProximityConfig);
    PerceptionComponent->SetDominantSense(ProximityConfig->GetSenseImplementation());
    PerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &ABallista::OnTargetPerceptionUpdated);
    CurrentTeam = EAITeam::RangedDefender;
    PrimaryRadiusColour = FLinearColor(1.0f, 0.0f, 0.0f, 1.0f);
}

void ABallista::BeginPlay()
{
    Super::BeginPlay();
    HitFlashComponent->BindMaterials();
    if (PivotPoint) DefaultPivotRotation = PivotPoint->GetRelativeRotation();
    if (OccupiedRadius <= 0.0f && BallistaMesh)
    {
        const FVector Extent = BallistaMesh->Bounds.BoxExtent;
        OccupiedRadius = FMath::Min(Extent.X, Extent.Y);
    }
}

void ABallista::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
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
    if (!IsValid(CurrentAttackTarget) || !bCanAttack) return;
    bCanAttack = false;
    FireProjectile();
    GetWorldTimerManager().SetTimer(AttackTimerHandle, [this]() -> void { bCanAttack = true; }, AttackCooldown, false);
}

void ABallista::DoOnSetActive(bool bActive)
{
    Super::DoOnSetActive(bActive);
    if (bActive)
    {
        CurrentAttackTarget = nullptr;
        GetWorldTimerManager().ClearTimer(AttackTimerHandle);
        VisiblePawns.Empty();
        PerceptionComponent->SetActive(true);
        if (UAIPerceptionSystem* PerceptionSys = UAIPerceptionSystem::GetCurrent(GetWorld()))
            PerceptionSys->UpdateListener(*PerceptionComponent);
    }
    else
    {
        PerceptionComponent->ForgetAll();
        VisiblePawns.Empty();
        PerceptionComponent->SetActive(false);
    }
    StandMesh->SetVisibility(bActive);
    StandMesh->SetComponentTickEnabled(bActive);
    StandMesh->SetCollisionEnabled(bActive ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision);
    StandMesh->SetCollisionResponseToAllChannels(bActive ? ECR_Block : ECR_Ignore);
    BallistaMesh->SetVisibility(bActive);
    BallistaMesh->SetComponentTickEnabled(bActive);
    BallistaMesh->SetCollisionEnabled(bActive ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision);
    BallistaMesh->SetCollisionResponseToAllChannels(bActive ? ECR_Block : ECR_Ignore);
    if (bActive)
    {
        StandMesh->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Ignore);
        StandMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
        BallistaMesh->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Ignore);
        BallistaMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
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
    return FMath::RadiansToDegrees(Theta);
}

void ABallista::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
    if (!IsValid(Actor)) return;
    if (ATowerDefencePawn* TDPawn = Cast<ATowerDefencePawn>(Actor))
    {
        if (!Stimulus.WasSuccessfullySensed()) VisiblePawns.Remove(TDPawn);
        else if (TDPawn->IsPawnActive() && TDPawn->GetHealthComponent()->IsAlive()) VisiblePawns.AddUnique(TDPawn);
    }
}

void ABallista::UpdateAttackTarget()
{
    if (const ATowerDefencePawn* Current = CurrentAttackTarget;
        IsValid(Current) && Current->IsPawnActive() && Current->GetHealthComponent()->IsAlive() &&
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
    if (!IsValid(CurrentAttackTarget) || !BoltClass) return;
    if (PROJECTILE_POOL_FACTORY_EXISTS)
    {
        const FVector LaunchLocation = MuzzlePoint->GetComponentLocation();
        const FVector TargetLocation = CurrentAttackTarget->GetVisualAttackPointLocation();
        ABallistaBolt* Bolt = Cast<ABallistaBolt>(CREATE_PROJECTILE(BoltClass, FTransform(LaunchLocation)));
        if (!Bolt) return;
        const FVector LaunchVelocity = ComputeLaunchVelocity(LaunchLocation, TargetLocation);
        Bolt->Fire(CurrentAttackTarget->GetVisualAttackPoint(), DamageComponent->GetDamage(), LaunchVelocity);
    }
}

FVector ABallista::ComputeLaunchVelocity(const FVector& LaunchLocation, const FVector& TargetLocation) const
{
    FVector HorizontalDirection = TargetLocation - LaunchLocation;
    HorizontalDirection.Z = 0.0f;
    if (!HorizontalDirection.Normalize()) return FVector::ZeroVector;
    const float ThetaDegrees =
        GetAngleToFireProjectile(this, LaunchLocation, TargetLocation, ProjectileSpeed, ProjectileGravityScale);
    const float ThetaRadians = FMath::DegreesToRadians(ThetaDegrees);
    return (HorizontalDirection * FMath::Cos(ThetaRadians) + FVector::UpVector * FMath::Sin(ThetaRadians)) *
           ProjectileSpeed;
}
