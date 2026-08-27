#pragma once

#include "CoreMinimal.h"

#include "Subsystems/GameInstanceSubsystem.h"

#include "TowerDefensePawnFactory.generated.h"

class UTowerDefensePawnFactory;

namespace Internal
{
    static UTowerDefensePawnFactory* GetTowerDefensePawnFactoryFromContext(const UObject* ContextObject)
    {
        if (!ContextObject || !IsValid(ContextObject)) return nullptr;
        const UWorld* World = nullptr;
        if (const AActor* Actor = Cast<AActor>(ContextObject)) World = Actor->GetWorld();
        else if (const USceneComponent* SceneComponent = Cast<USceneComponent>(ContextObject))
            World = SceneComponent->GetWorld();
        else if (const UActorComponent* ActorComponent = Cast<UActorComponent>(ContextObject))
        {
            if (const AActor* Owner = ActorComponent->GetOwner()) World = Owner->GetWorld();
        }
        if (!World) return nullptr;
        if (const UGameInstance* GameInstance = World->GetGameInstance())
            return GameInstance->GetSubsystem<UTowerDefensePawnFactory>();
        return nullptr;
    }
} // namespace Internal

#ifndef TOWER_DEFENSE_PAWN_FACTORY_EXISTS
#define TOWER_DEFENSE_PAWN_FACTORY_EXISTS \
    UTowerDefensePawnFactory* TowerDefensePawnFactory = Internal::GetTowerDefensePawnFactoryFromContext(this)
#endif

#ifndef CREATE_PAWN
#define CREATE_PAWN(TowerDefensePawnBlueprint, SpawnTransform) \
    TowerDefensePawnFactory->CreatePawn(TowerDefensePawnBlueprint, SpawnTransform)
#endif

#ifndef DESTROY_PAWN
#define DESTROY_PAWN(TowerDefensePawn) TowerDefensePawnFactory->DestroyPawn(TowerDefensePawn)
#endif

class ATowerDefensePawn;

UCLASS()
class GADE7322_TD_API UTowerDefensePawnFactory : public UGameInstanceSubsystem
{
    GENERATED_BODY()

private:
    using PoolType = TSet<ATowerDefensePawn*>;
    using SizeType = PoolType::SizeType;

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

public:
    UFUNCTION(BlueprintCallable)
    ATowerDefensePawn* CreatePawn(const TSubclassOf<ATowerDefensePawn>& TowerDefensePawnBlueprint,
                                  const FTransform& SpawnTransform);

    UFUNCTION(BlueprintCallable)
    void DestroyPawn(ATowerDefensePawn* TowerDefensePawn);

private:
    void CreatePool(const TSubclassOf<ATowerDefensePawn>& TowerDefensePawnBlueprint);

private:
    TMap<TSubclassOf<ATowerDefensePawn>, PoolType> AvailablePawns;
    TMap<TSubclassOf<ATowerDefensePawn>, PoolType> UnavailablePawns;

    const SizeType StartingPoolSize = 10;
};
