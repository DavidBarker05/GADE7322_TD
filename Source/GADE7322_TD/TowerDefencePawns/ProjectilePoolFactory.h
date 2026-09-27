#pragma once

#include "CoreMinimal.h"

#include "Engine.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "UObject/StrongObjectPtr.h"

#include "ProjectilePoolFactory.generated.h"

#ifndef PROJECTILE_POOL_FACTORY_EXISTS
#define PROJECTILE_POOL_FACTORY_EXISTS \
    UProjectilePoolFactory* ProjectilePoolFactory = [](const UWorld* World) -> UProjectilePoolFactory* \
    { \
        if (World; const UGameInstance* GameInstance = World->GetGameInstance()) \
            return GameInstance->GetSubsystem<UProjectilePoolFactory>(); \
        return nullptr; \
    }(GEngine->GetWorldFromContextObject(this, EGetWorldErrorMode::LogAndReturnNull))
#endif
// ^ Same hacky one-liner as TOWER_DEFENCE_PAWN_FACTORY_EXISTS

#ifndef CREATE_PROJECTILE
#define CREATE_PROJECTILE(ProjectileClass, SpawnTransform) \
    ProjectilePoolFactory->CreateProjectile(ProjectileClass, SpawnTransform)
#endif

#ifndef RETURN_PROJECTILE
#define RETURN_PROJECTILE(Projectile) ProjectilePoolFactory->ReturnProjectile(Projectile)
#endif

#ifndef CLEAR_PROJECTILE_POOLS
#define CLEAR_PROJECTILE_POOLS() ProjectilePoolFactory->ClearAllPools()
#endif

UCLASS()
class GADE7322_TD_API UProjectilePoolFactory : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Deinitialize() override;

    UFUNCTION(BlueprintCallable)
    AActor* CreateProjectile(const TSubclassOf<AActor>& ProjectileClass, const FTransform& SpawnTransform);

    UFUNCTION(BlueprintCallable)
    void ReturnProjectile(AActor* Projectile);

    UFUNCTION(BlueprintCallable)
    void ClearAllPools();

private:
    void CreatePool(const TSubclassOf<AActor>& ProjectileClass);

    static void DeactivateProjectile(AActor* Projectile);

    TMap<TSubclassOf<AActor>, TSet<TStrongObjectPtr<AActor>>> AvailablePools;
    TMap<TSubclassOf<AActor>, TSet<TStrongObjectPtr<AActor>>> UnavailablePools;

    const int32 StartingPoolSize = 10;
};
