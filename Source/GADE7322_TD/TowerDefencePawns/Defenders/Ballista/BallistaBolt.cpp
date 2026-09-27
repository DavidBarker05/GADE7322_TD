#include "TowerDefencePawns/Defenders/Ballista/BallistaBolt.h"

#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "TowerDefencePawns/Attackers/Attacker.h"
#include "TowerDefencePawns/Components/HealthComponent.h"
#include "TowerDefencePawns/ProjectilePoolFactory.h"

ABallistaBolt::ABallistaBolt()
{
    PrimaryActorTick.bCanEverTick = true;
    BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Collision"));
    SetRootComponent(BoxCollision); // Projectile movement component needs collision root
    BoxCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    // ^ Hits are detected by distance to the target's VisualAttackPoint
    ProjectileMovementComponent =
        CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement Component"));
    ProjectileMovementComponent->bRotationFollowsVelocity = true;
    ProjectileMovementComponent->bShouldBounce = false;
    ProjectileMovementComponent->bIsHomingProjectile =
        true; // Homing because target might be too fast and move out of way
    StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh Component"));
    StaticMeshComponent->SetupAttachment(RootComponent);
}

void ABallistaBolt::BeginPlay() { Super::BeginPlay(); }

void ABallistaBolt::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    if (!IsValid(Target)) return;
    if (FVector::DistSquared(GetActorLocation(), Target->GetComponentLocation()) > FMath::Square(HitRadius)) return;
    if (AAttacker* HitAttacker = Cast<AAttacker>(Target->GetOwner());
        IsValid(HitAttacker) && HitAttacker->IsPawnActive() && HitAttacker->GetHealthComponent()->IsAlive())
        HitAttacker->GetHealthComponent()->TakeDamage(Damage);
    ReturnToPool();
}

ABallistaBolt& ABallistaBolt::SetTarget(USceneComponent* InTarget)
{
    ProjectileMovementComponent->bIsHomingProjectile = IsValid(InTarget);
    ProjectileMovementComponent->HomingTargetComponent = InTarget;
    Target = InTarget;
    return *this;
}

void ABallistaBolt::Fire(USceneComponent* InTarget, int32 InDamage, const FVector& LaunchVelocity)
{
    SetTarget(InTarget);
    SetDamage(InDamage);
    ProjectileMovementComponent->StopMovementImmediately();
    ProjectileMovementComponent->Velocity = LaunchVelocity;
    GetWorldTimerManager().SetTimer(LifespanHandle, this, &ABallistaBolt::ReturnToPool, MaxLifetime, false);
}

void ABallistaBolt::ReturnToPool()
{
    GetWorldTimerManager().ClearTimer(LifespanHandle);
    SetTarget(nullptr);
    ProjectileMovementComponent->StopMovementImmediately();
    if (PROJECTILE_POOL_FACTORY_EXISTS) RETURN_PROJECTILE(this);
}
