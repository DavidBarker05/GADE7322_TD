#include "TowerDefencePawns/Attackers/Mage/DamageSpellProjectile.h"

#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "NiagaraComponent.h"
#include "TowerDefencePawns/Components/HealthComponent.h"
#include "TowerDefencePawns/ProjectilePoolFactory.h"
#include "TowerDefencePawns/TowerDefencePawn.h"

ADamageSpellProjectile::ADamageSpellProjectile()
{
    PrimaryActorTick.bCanEverTick = true;
    SphereCollider = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Collider"));
    SetRootComponent(SphereCollider); // Projectile movement requires collider root
    SphereCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision); // Going to be distance-based
    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement"));
    ProjectileMovement->bRotationFollowsVelocity = true;
    ProjectileMovement->bShouldBounce = false;
    ProjectileMovement->bIsHomingProjectile = true;
    ProjectileMovement->ProjectileGravityScale = 0.0f;
    SpellEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Spell Effect"));
    SpellEffect->SetupAttachment(RootComponent);
}

void ADamageSpellProjectile::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    if (!IsValid(Target)) return;
    if (FVector::DistSquared(GetActorLocation(), Target->GetComponentLocation()) > FMath::Square(HitDetectionRadius))
        return;
    if (ATowerDefencePawn* HitPawn = Cast<ATowerDefencePawn>(Target->GetOwner());
        IsValid(HitPawn) && HitPawn->IsPawnActive() && HitPawn->GetHealthComponent()->IsAlive())
        HitPawn->GetHealthComponent()->TakeDamage(Damage);
    ReturnToPool();
}

void ADamageSpellProjectile::Fire(USceneComponent* InTarget, int32 InDamage, const FVector& LaunchVelocity)
{
    SetTarget(InTarget);
    Damage = InDamage;
    ProjectileMovement->StopMovementImmediately();
    ProjectileMovement->Velocity = LaunchVelocity;
    GetWorldTimerManager().SetTimer(LifespanHandle, this, &ADamageSpellProjectile::ReturnToPool, MaxLifetime, false);
}

void ADamageSpellProjectile::SetTarget(USceneComponent* InTarget)
{
    ProjectileMovement->bIsHomingProjectile = IsValid(InTarget);
    ProjectileMovement->HomingTargetComponent = InTarget;
    Target = InTarget;
}

void ADamageSpellProjectile::ReturnToPool()
{
    GetWorldTimerManager().ClearTimer(LifespanHandle);
    SetTarget(nullptr);
    ProjectileMovement->StopMovementImmediately();
    if (PROJECTILE_POOL_FACTORY_EXISTS) RETURN_PROJECTILE(this);
}
