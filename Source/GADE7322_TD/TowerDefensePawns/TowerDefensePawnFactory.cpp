#include "TowerDefensePawns/TowerDefensePawnFactory.h"

#include "TowerDefensePawns/TowerDefensePawn.h"

void UTowerDefensePawnFactory::Initialize(FSubsystemCollectionBase& Collection) { Super::Initialize(Collection); }

void UTowerDefensePawnFactory::Deinitialize()
{
    AvailablePawns.Empty();
    UnavailablePawns.Empty();
    Super::Deinitialize();
}

ATowerDefensePawn* UTowerDefensePawnFactory::CreatePawn(const TSubclassOf<ATowerDefensePawn>& TowerDefensePawnBlueprint,
                                                        const FTransform& SpawnTransform)
{
    if (!IsValid(TowerDefensePawnBlueprint)) return nullptr;
    if (!AvailablePawns.Contains(TowerDefensePawnBlueprint)) CreatePool(TowerDefensePawnBlueprint);
    ATowerDefensePawn** PawnPtr = AvailablePawns[TowerDefensePawnBlueprint].FindArbitraryElement();
    ATowerDefensePawn* Pawn =
        PawnPtr ? *PawnPtr : GetWorld()->SpawnActor<ATowerDefensePawn>(TowerDefensePawnBlueprint, SpawnTransform);
    UnavailablePawns[TowerDefensePawnBlueprint].Emplace(Pawn);
    if (PawnPtr)
    {
        Pawn->SetActorTransform(SpawnTransform, false, nullptr, ETeleportType::ResetPhysics);
        AvailablePawns[TowerDefensePawnBlueprint].Remove(Pawn);
    }
    return Pawn;
}

void UTowerDefensePawnFactory::DestroyPawn(ATowerDefensePawn* TowerDefensePawn)
{
    if (!IsValid(TowerDefensePawn)) return;
    if (!UnavailablePawns.Contains(TowerDefensePawn->GetClass())) return;
    if (!UnavailablePawns[TowerDefensePawn->GetClass()].Contains(TowerDefensePawn)) return;
    AvailablePawns[TowerDefensePawn->GetClass()].Add(TowerDefensePawn);
    UnavailablePawns[TowerDefensePawn->GetClass()].Remove(TowerDefensePawn);
}

void UTowerDefensePawnFactory::CreatePool(const TSubclassOf<ATowerDefensePawn>& TowerDefensePawnBlueprint)
{
    AvailablePawns.Add(TowerDefensePawnBlueprint);
    AvailablePawns[TowerDefensePawnBlueprint].Reserve(StartingPoolSize);
    for (SizeType i = 0; i < StartingPoolSize; ++i)
    {
        AvailablePawns[TowerDefensePawnBlueprint].Emplace(
            GetWorld()->SpawnActor<ATowerDefensePawn>(TowerDefensePawnBlueprint));
    }
    UnavailablePawns.Add(TowerDefensePawnBlueprint);
    UnavailablePawns.Reserve(StartingPoolSize);
}
