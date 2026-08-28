#pragma once

#include "CoreMinimal.h"

#include "Subsystems/GameInstanceSubsystem.h"

#include "TowerDefencePawnFactory.generated.h"

class UTowerDefencePawnFactory;

namespace Internal
{
    static UTowerDefencePawnFactory* GetTowerDefencePawnFactoryFromContext(const UObject* ContextObject)
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
            return GameInstance->GetSubsystem<UTowerDefencePawnFactory>();
        return nullptr;
    }
} // namespace Internal

#ifndef TOWER_DEFENCE_PAWN_FACTORY_EXISTS
#define TOWER_DEFENCE_PAWN_FACTORY_EXISTS \
    UTowerDefencePawnFactory* TowerDefencePawnFactory = Internal::GetTowerDefencePawnFactoryFromContext(this)
#endif

#ifndef CREATE_PAWN
#define CREATE_PAWN(TowerDefencePawnBlueprint, SpawnTransform) \
    TowerDefencePawnFactory->CreatePawn(TowerDefencePawnBlueprint, SpawnTransform)
#endif

#ifndef DESTROY_PAWN
#define DESTROY_PAWN(TowerDefencePawn) TowerDefencePawnFactory->DestroyPawn(TowerDefencePawn)
#endif

class ATowerDefencePawn;

UCLASS()
class GADE7322_TD_API UTowerDefencePawnFactory : public UGameInstanceSubsystem
{
    GENERATED_BODY()

    using FPoolType = TSet<ATowerDefencePawn*>;
    using SizeType = FPoolType::SizeType;

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    UFUNCTION(BlueprintCallable)
    ATowerDefencePawn* CreatePawn(const TSubclassOf<ATowerDefencePawn>& TowerDefencePawnBlueprint,
                                  const FTransform& SpawnTransform);

    UFUNCTION(BlueprintCallable)
    void DestroyPawn(ATowerDefencePawn* TowerDefencePawn);

private:
    void CreatePool(const TSubclassOf<ATowerDefencePawn>& TowerDefencePawnBlueprint);

    TMap<TSubclassOf<ATowerDefencePawn>, FPoolType> AvailablePawns;
    TMap<TSubclassOf<ATowerDefencePawn>, FPoolType> UnavailablePawns;

    const SizeType StartingPoolSize = 10;
};
