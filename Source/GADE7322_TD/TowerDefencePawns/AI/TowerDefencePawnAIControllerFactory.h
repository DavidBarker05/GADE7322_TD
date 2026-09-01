#pragma once

#include "CoreMinimal.h"

#include "Subsystems/GameInstanceSubsystem.h"
#include "UObject/StrongObjectPtr.h"

#include "TowerDefencePawnAIControllerFactory.generated.h"

#ifndef POSSESS_TOWER_DEFENCE_PAWN
#define POSSESS_TOWER_DEFENCE_PAWN(TowerDefencePawn) \
    do \
    { \
        if (const UWorld* World = this->GetWorld(); const UGameInstance* GameInstance = World->GetGameInstance()) \
        { \
            if (UTowerDefencePawnAIControllerFactory* TowerDefencePawnAIControllerFactory = \
                    GameInstance->GetSubsystem<UTowerDefencePawnAIControllerFactory>()) \
                TowerDefencePawnAIControllerFactory->PossessTowerDefencePawn(TowerDefencePawn); \
        } \
    } while (0)
#endif

#ifndef UNPOSSESS_TOWER_DEFENCE_PAWN
#define UNPOSSESS_TOWER_DEFENCE_PAWN(TowerDefencePawn) \
    do \
    { \
        if (const UWorld* World = this->GetWorld(); const UGameInstance* GameInstance = World->GetGameInstance()) \
        { \
            if (UTowerDefencePawnAIControllerFactory* TowerDefencePawnAIControllerFactory = \
                    GameInstance->GetSubsystem<UTowerDefencePawnAIControllerFactory>()) \
                TowerDefencePawnAIControllerFactory->UnpossessTowerDefencePawn(TowerDefencePawn); \
        } \
    } while (0)
#endif

class ATowerDefencePawn;
class ATowerDefencePawnAIController;

UCLASS()
class GADE7322_TD_API UTowerDefencePawnAIControllerFactory : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Deinitialize() override;

    UFUNCTION(BlueprintCallable)
    void PossessTowerDefencePawn(ATowerDefencePawn* TowerDefencePawn);

    UFUNCTION(BlueprintCallable)
    void UnpossessTowerDefencePawn(ATowerDefencePawn* TowerDefencePawn);

private:
    void CreatePool(const TSubclassOf<ATowerDefencePawnAIController>& AIControllerClass);

    TMap<TSubclassOf<ATowerDefencePawnAIController>, TSet<TStrongObjectPtr<ATowerDefencePawnAIController>>>
        AvailableControllerPools;
    TMap<TSubclassOf<ATowerDefencePawnAIController>, TSet<TStrongObjectPtr<ATowerDefencePawnAIController>>>
        UnavailableControllerPools;

    const int32 StartingPoolSize = 10;
};
