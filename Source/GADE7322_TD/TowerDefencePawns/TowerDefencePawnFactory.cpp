#include "TowerDefencePawns/TowerDefencePawnFactory.h"

#include "TowerDefencePawns/TowerDefencePawn.h"

void UTowerDefencePawnFactory::Initialize(FSubsystemCollectionBase& Collection) { Super::Initialize(Collection); }

void UTowerDefencePawnFactory::Deinitialize()
{
    AvailablePawns.Empty();
    UnavailablePawns.Empty();
    Super::Deinitialize();
}

ATowerDefencePawn* UTowerDefencePawnFactory::CreatePawn(const TSubclassOf<ATowerDefencePawn>& TowerDefencePawnBlueprint,
                                                        const FTransform& SpawnTransform)
{
    if (!IsValid(TowerDefencePawnBlueprint)) return nullptr;
    if (!AvailablePawns.Contains(TowerDefencePawnBlueprint)) CreatePool(TowerDefencePawnBlueprint);
    ATowerDefencePawn** PawnPtr = AvailablePawns[TowerDefencePawnBlueprint].FindArbitraryElement();
    ATowerDefencePawn* Pawn =
        PawnPtr ? *PawnPtr : GetWorld()->SpawnActor<ATowerDefencePawn>(TowerDefencePawnBlueprint, SpawnTransform);
    UnavailablePawns[TowerDefencePawnBlueprint].Emplace(Pawn);
    if (PawnPtr)
    {
        Pawn->SetActorTransform(SpawnTransform, false, nullptr, ETeleportType::ResetPhysics);
        AvailablePawns[TowerDefencePawnBlueprint].Remove(Pawn);
    }
    return Pawn;
}

void UTowerDefencePawnFactory::DestroyPawn(ATowerDefencePawn* TowerDefencePawn)
{
    if (!IsValid(TowerDefencePawn)) return;
    if (!UnavailablePawns.Contains(TowerDefencePawn->GetClass())) return;
    if (!UnavailablePawns[TowerDefencePawn->GetClass()].Contains(TowerDefencePawn)) return;
    AvailablePawns[TowerDefencePawn->GetClass()].Add(TowerDefencePawn);
    UnavailablePawns[TowerDefencePawn->GetClass()].Remove(TowerDefencePawn);
}

void UTowerDefencePawnFactory::CreatePool(const TSubclassOf<ATowerDefencePawn>& TowerDefencePawnBlueprint)
{
    AvailablePawns.Add(TowerDefencePawnBlueprint);
    AvailablePawns[TowerDefencePawnBlueprint].Reserve(StartingPoolSize);
    for (SizeType i = 0; i < StartingPoolSize; ++i)
    {
        AvailablePawns[TowerDefencePawnBlueprint].Emplace(
            GetWorld()->SpawnActor<ATowerDefencePawn>(TowerDefencePawnBlueprint));
    }
    UnavailablePawns.Add(TowerDefencePawnBlueprint);
    UnavailablePawns.Reserve(StartingPoolSize);
}
