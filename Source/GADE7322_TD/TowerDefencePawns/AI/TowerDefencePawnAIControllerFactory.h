#pragma once

#include "CoreMinimal.h"

#include "Subsystems/GameInstanceSubsystem.h"
#include "UObject/StrongObjectPtr.h"

#include "TowerDefencePawnAIControllerFactory.generated.h"

class UTowerDefencePawnAIControllerFactory;

namespace Internal
{
    static UTowerDefencePawnAIControllerFactory*
    GetTowerDefencePawnAIControllerFactoryFromContext(const UObject* ContextObject)
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
            return GameInstance->GetSubsystem<UTowerDefencePawnAIControllerFactory>();
        return nullptr;
    }
} // namespace Internal

#ifndef POSSESS_TOWER_DEFENCE_PAWN
#define POSSESS_TOWER_DEFENCE_PAWN(TowerDefencePawn) \
    if (UTowerDefencePawnAIControllerFactory* TowerDefencePawnAIControllerFactory = \
            Internal::GetTowerDefencePawnAIControllerFactoryFromContext(this)) \
    TowerDefencePawnAIControllerFactory->PossessTowerDefencePawn(TowerDefencePawn)
#endif

#ifndef UNPOSSESS_TOWER_DEFENCE_PAWN
#define UNPOSSESS_TOWER_DEFENCE_PAWN(TowerDefencePawn) \
    if (UTowerDefencePawnAIControllerFactory* TowerDefencePawnAIControllerFactory = \
            Internal::GetTowerDefencePawnAIControllerFactoryFromContext(this)) \
    TowerDefencePawnAIControllerFactory->UnpossessTowerDefencePawn(TowerDefencePawn)
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
