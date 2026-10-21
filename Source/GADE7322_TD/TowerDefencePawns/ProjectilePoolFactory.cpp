#include "TowerDefencePawns/ProjectilePoolFactory.h"

#include "GameFramework/ProjectileMovementComponent.h"

void UProjectilePoolFactory::Deinitialize()
{
    ClearAllPools();
    Super::Deinitialize();
}

void UProjectilePoolFactory::ClearAllPools()
{
    AvailablePools.Empty();
    UnavailablePools.Empty();
}

AActor* UProjectilePoolFactory::CreateProjectile(const TSubclassOf<AActor>& ProjectileClass,
                                                 const FTransform& SpawnTransform)
{
    if (!IsValid(ProjectileClass)) return nullptr;
    if (!AvailablePools.Contains(ProjectileClass)) CreatePool(ProjectileClass);
    auto& AvailablePool = AvailablePools[ProjectileClass];
    AActor* Projectile = nullptr;
    while (!Projectile)
    {
        const auto* ProjectilePtr = AvailablePool.FindArbitraryElement();
        if (!ProjectilePtr) break;
        AActor* Candidate = ProjectilePtr->Get();
        AvailablePool.Remove(*ProjectilePtr);
        if (IsValid(Candidate) && Candidate->GetWorld() == GetWorld()) Projectile = Candidate;
    }
    if (Projectile)
    {
        Projectile->SetActorTransform(SpawnTransform, false, nullptr, ETeleportType::ResetPhysics);
        Projectile->SetActorHiddenInGame(false);
        Projectile->SetActorEnableCollision(true);
        Projectile->SetActorTickEnabled(true);
        if (UProjectileMovementComponent* Movement = Projectile->FindComponentByClass<UProjectileMovementComponent>())
            Movement->Activate();
    }
    else
    {
        FActorSpawnParameters SpawnParams;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
        Projectile = GetWorld()->SpawnActor<AActor>(ProjectileClass, SpawnTransform, SpawnParams);
    }
    if (Projectile) UnavailablePools[ProjectileClass].Emplace(Projectile);
    return Projectile;
}

void UProjectilePoolFactory::ReturnProjectile(AActor* Projectile)
{
    if (!IsValid(Projectile)) return;
    if (!UnavailablePools.Contains(Projectile->GetClass())) return;
    const TStrongObjectPtr<AActor> StrongProjectile(Projectile);
    auto& UnavailablePool = UnavailablePools[Projectile->GetClass()];
    if (!UnavailablePool.Contains(StrongProjectile)) return;
    DeactivateProjectile(Projectile);
    AvailablePools[Projectile->GetClass()].Add(StrongProjectile);
    UnavailablePool.Remove(StrongProjectile);
}

void UProjectilePoolFactory::CreatePool(const TSubclassOf<AActor>& ProjectileClass)
{
    AvailablePools.Add(ProjectileClass);
    AvailablePools[ProjectileClass].Reserve(StartingPoolSize);
    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    for (int32 i = 0; i < StartingPoolSize; ++i)
    {
        if (AActor* Projectile = GetWorld()->SpawnActor<AActor>(ProjectileClass, FTransform::Identity, SpawnParams))
        {
            DeactivateProjectile(Projectile);
            AvailablePools[ProjectileClass].Emplace(Projectile);
        }
    }
    UnavailablePools.Add(ProjectileClass);
    UnavailablePools[ProjectileClass].Reserve(StartingPoolSize);
}

void UProjectilePoolFactory::DeactivateProjectile(AActor* Projectile)
{
    Projectile->SetActorHiddenInGame(true);
    Projectile->SetActorEnableCollision(false);
    Projectile->SetActorTickEnabled(false);
    if (UProjectileMovementComponent* Movement = Projectile->FindComponentByClass<UProjectileMovementComponent>())
    {
        Movement->StopMovementImmediately();
        Movement->Deactivate();
    }
}
