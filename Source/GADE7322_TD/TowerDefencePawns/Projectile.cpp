#include "TowerDefencePawns/Projectile.h"

#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "TowerDefencePawns/Components/HealthComponent.h"
#include "TowerDefencePawns/ProjectilePoolFactory.h"
#include "TowerDefencePawns/TowerDefencePawn.h"

AProjectile::AProjectile()
{
    PrimaryActorTick.bCanEverTick = true;
    Root = CreateDefaultSubobject<USphereComponent>(TEXT("Root"));
    SetRootComponent(Root); // Projectile movement component needs collision root
    Root->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    // ^ Hits are detected by distance to the target's VisualAttackPoint
    ProjectileMovementComponent =
        CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement Component"));
    ProjectileMovementComponent->bRotationFollowsVelocity = true;
    ProjectileMovementComponent->bShouldBounce = false;
    ProjectileMovementComponent->bIsHomingProjectile = true;
    StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh Component"));
    StaticMeshComponent->SetupAttachment(RootComponent);
}

void AProjectile::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    if (!IsValid(Target)) return;
    if (FVector::DistSquared(GetActorLocation(), Target->GetComponentLocation()) > FMath::Square(HitRadius)) return;
    if (ATowerDefencePawn* HitPawn = Cast<ATowerDefencePawn>(Target->GetOwner());
        IsValid(HitPawn) && HitPawn->IsPawnActive() && HitPawn->GetHealthComponent()->IsAlive())
    {
        DamageTarget(HitPawn);
        ReturnToPool();
    }
}

void AProjectile::FireProjectile(USceneComponent* InTarget, const FVector& LaunchVelocity, int32 InDamage,
                                 const TArray<FAny>& LaunchArguments)
{
    if (!IsValid(InTarget) || InDamage <= 0) return;
    FireWithoutLaunch(InTarget, InDamage);
    Launch(LaunchVelocity);
}

void AProjectile::FireWithoutLaunch(USceneComponent* InTarget, int32 InDamage)
{
    if (!IsValid(InTarget) || InDamage <= 0) return;
    ProjectileMovementComponent->HomingTargetComponent = InTarget;
    Target = InTarget;
    Damage = InDamage;
    ProjectileMovementComponent->StopMovementImmediately();
}

void AProjectile::DamageTarget(ATowerDefencePawn* TargetToDamage)
{
    TargetToDamage->GetHealthComponent()->TakeDamage(Damage);
}

void AProjectile::Launch(const FVector& LaunchVelocity)
{
    ProjectileMovementComponent->Velocity = LaunchVelocity;
    GetWorldTimerManager().SetTimer(LifespanHandle, this, &AProjectile::ReturnToPool, MaxLifetime, false);
}

void AProjectile::ReturnToPool()
{
    GetWorldTimerManager().ClearTimer(LifespanHandle);
    ProjectileMovementComponent->HomingTargetComponent = nullptr;
    ProjectileMovementComponent->StopMovementImmediately();
    OnReturnToPool();
    if (PROJECTILE_POOL_FACTORY_EXISTS) RETURN_PROJECTILE(this);
}
