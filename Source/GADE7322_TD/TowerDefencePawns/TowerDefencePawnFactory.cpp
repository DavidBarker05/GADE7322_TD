#include "TowerDefencePawns/TowerDefencePawnFactory.h"

#include "TowerDefencePawns/TowerDefencePawn.h"

void UTowerDefencePawnFactory::Deinitialize()
{
    AvailablePawnPools.Empty();
    UnavailablePawnPools.Empty();
    Super::Deinitialize();
}

ATowerDefencePawn* UTowerDefencePawnFactory::CreatePawn(const TSubclassOf<ATowerDefencePawn>& TowerDefencePawnBlueprint,
                                                        const FTransform& SpawnTransform)
{
    if (!IsValid(TowerDefencePawnBlueprint)) return nullptr;
    if (!AvailablePawnPools.Contains(TowerDefencePawnBlueprint)) CreatePool(TowerDefencePawnBlueprint);
    const FTransform SpawnLocationAndRotation(SpawnTransform.GetRotation(), SpawnTransform.GetLocation());
    TSet<TStrongObjectPtr<ATowerDefencePawn>>& AvailablePool = AvailablePawnPools[TowerDefencePawnBlueprint];
    TStrongObjectPtr<ATowerDefencePawn>* PawnPtr = AvailablePool.FindArbitraryElement();
    ATowerDefencePawn* Pawn =
        PawnPtr ? PawnPtr->Get() :
                  GetWorld()->SpawnActor<ATowerDefencePawn>(TowerDefencePawnBlueprint, SpawnLocationAndRotation);
    UnavailablePawnPools[TowerDefencePawnBlueprint].Emplace(Pawn);
    if (PawnPtr)
    {
        AvailablePool.Remove(*PawnPtr);
        Pawn->SetActorTransform(SpawnLocationAndRotation, false, nullptr, ETeleportType::ResetPhysics);
    }
    return Pawn;
}

void UTowerDefencePawnFactory::DestroyPawn(ATowerDefencePawn* TowerDefencePawn)
{
    if (!IsValid(TowerDefencePawn)) return;
    if (!UnavailablePawnPools.Contains(TowerDefencePawn->GetClass())) return;
    const TStrongObjectPtr<ATowerDefencePawn> StrongPawn(TowerDefencePawn);
    TSet<TStrongObjectPtr<ATowerDefencePawn>>& UnavailablePool = UnavailablePawnPools[TowerDefencePawn->GetClass()];
    if (!UnavailablePool.Contains(StrongPawn)) return;
    AvailablePawnPools[TowerDefencePawn->GetClass()].Add(StrongPawn);
    UnavailablePool.Remove(StrongPawn);
}

void UTowerDefencePawnFactory::CreatePool(const TSubclassOf<ATowerDefencePawn>& TowerDefencePawnBlueprint)
{
    AvailablePawnPools.Add(TowerDefencePawnBlueprint);
    AvailablePawnPools[TowerDefencePawnBlueprint].Reserve(StartingPoolSize);
    for (int32 i = 0; i < StartingPoolSize; ++i)
    {
        AvailablePawnPools[TowerDefencePawnBlueprint].Emplace(
            GetWorld()->SpawnActor<ATowerDefencePawn>(TowerDefencePawnBlueprint));
    }
    UnavailablePawnPools.Add(TowerDefencePawnBlueprint);
    UnavailablePawnPools[TowerDefencePawnBlueprint].Reserve(StartingPoolSize);
}
