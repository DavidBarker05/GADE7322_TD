#include "TowerDefencePawns/AI/TowerDefencePawnAIControllerFactory.h"
#include "TowerDefencePawns/AI/TowerDefencePawnAIController.h"
#include "TowerDefencePawns/TowerDefencePawn.h"

void UTowerDefencePawnAIControllerFactory::Deinitialize()
{
    AvailableControllerPools.Empty();
    UnavailableControllerPools.Empty();
    Super::Deinitialize();
}

void UTowerDefencePawnAIControllerFactory::PossessTowerDefencePawn(ATowerDefencePawn* TowerDefencePawn)
{
    if (!IsValid(TowerDefencePawn)) return;
    const auto ControllerClass = TowerDefencePawn->GetTDAIControllerClass();
    if (!AvailableControllerPools.Contains(ControllerClass)) CreatePool(ControllerClass);
    auto& AvailablePool = AvailableControllerPools[ControllerClass];
    const auto* ControllerPtr = AvailablePool.FindArbitraryElement();
    auto* Controller =
        ControllerPtr ? ControllerPtr->Get() : GetWorld()->SpawnActor<ATowerDefencePawnAIController>(ControllerClass);
    UnavailableControllerPools[ControllerClass].Emplace(Controller);
    if (ControllerPtr) AvailablePool.Remove(*ControllerPtr);
    Controller->Possess(TowerDefencePawn);
}

void UTowerDefencePawnAIControllerFactory::UnpossessTowerDefencePawn(ATowerDefencePawn* TowerDefencePawn)
{
    if (!IsValid(TowerDefencePawn)) return;
    auto* Controller = Cast<ATowerDefencePawnAIController>(TowerDefencePawn->GetController());
    if (!Controller) return;
    if (!UnavailableControllerPools.Contains(Controller->GetClass())) return;
    const TStrongObjectPtr<ATowerDefencePawnAIController> StrongController(Controller);
    auto& UnavailablePool = UnavailableControllerPools[Controller->GetClass()];
    if (!UnavailablePool.Contains(StrongController)) return;
    AvailableControllerPools[Controller->GetClass()].Add(StrongController);
    UnavailablePool.Remove(StrongController);
    Controller->UnPossess();
}

void UTowerDefencePawnAIControllerFactory::CreatePool(
    const TSubclassOf<ATowerDefencePawnAIController>& AIControllerClass)
{
    AvailableControllerPools.Add(AIControllerClass);
    AvailableControllerPools[AIControllerClass].Reserve(StartingPoolSize);
    for (int32 i = 0; i < StartingPoolSize; ++i)
    {
        AvailableControllerPools[AIControllerClass].Emplace(
            GetWorld()->SpawnActor<ATowerDefencePawnAIController>(AIControllerClass));
    }
    UnavailableControllerPools.Add(AIControllerClass);
    UnavailableControllerPools[AIControllerClass].Reserve(StartingPoolSize);
}
