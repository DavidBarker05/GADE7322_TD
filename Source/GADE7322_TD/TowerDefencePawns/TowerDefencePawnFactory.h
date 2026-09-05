#pragma once

#include "CoreMinimal.h"

#include "Engine.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "UObject/StrongObjectPtr.h"

#include "TowerDefencePawnFactory.generated.h"

#ifndef TOWER_DEFENCE_PAWN_FACTORY_EXISTS
#define TOWER_DEFENCE_PAWN_FACTORY_EXISTS \
    UTowerDefencePawnFactory* TowerDefencePawnFactory = [](const UWorld* World) -> UTowerDefencePawnFactory* \
    { \
        if (World; const UGameInstance* GameInstance = World->GetGameInstance()) \
            return GameInstance->GetSubsystem<UTowerDefencePawnFactory>(); \
        return nullptr; \
    }(GEngine->GetWorldFromContextObject(this, EGetWorldErrorMode::LogAndReturnNull))
#endif
// ^ This is very hacky way around of not being able to do:
// ---
// const UWorld* World = this->GetWorld(); const UGameInstance* GameInstance = World->GetGameInstance();
// UTowerDefencePawnFactory* TowerDefencePawnFactory = GameInstance->GetSubsystem<UTowerDefencePawnFactory>()
// ---
// inside one if statement

#ifndef CREATE_PAWN
#define CREATE_PAWN(TowerDefencePawnBlueprint, SpawnTransform) \
    TowerDefencePawnFactory->CreatePawn(TowerDefencePawnBlueprint, SpawnTransform)
#endif

#ifndef DESTROY_PAWN
#define DESTROY_PAWN(TowerDefencePawn) TowerDefencePawnFactory->DestroyPawn(TowerDefencePawn)
#endif

#ifndef CLEAR_PAWN_POOLS
#define CLEAR_PAWN_POOLS() TowerDefencePawnFactory->ClearAllPools()
#endif

class ATowerDefencePawn;

UCLASS()
class GADE7322_TD_API UTowerDefencePawnFactory : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Deinitialize() override;

    UFUNCTION(BlueprintCallable)
    ATowerDefencePawn* CreatePawn(const TSubclassOf<ATowerDefencePawn>& TowerDefencePawnBlueprint,
                                  const FTransform& SpawnTransform);

    UFUNCTION(BlueprintCallable)
    void DestroyPawn(ATowerDefencePawn* TowerDefencePawn);

    UFUNCTION(BlueprintCallable)
    void ClearAllPools();

private:
    void CreatePool(const TSubclassOf<ATowerDefencePawn>& TowerDefencePawnBlueprint);

    TMap<TSubclassOf<ATowerDefencePawn>, TSet<TStrongObjectPtr<ATowerDefencePawn>>> AvailablePawnPools;
    TMap<TSubclassOf<ATowerDefencePawn>, TSet<TStrongObjectPtr<ATowerDefencePawn>>> UnavailablePawnPools;

    const int32 StartingPoolSize = 10;
};
