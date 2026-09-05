#include "TowerDefencePawns/TowerDefencePawnFactory.h"

#include "Components/CapsuleComponent.h"
#include "CustomLog.h"
#include "GameFramework/CharacterMovementComponent.h"
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
    const float HalfHeight =
        TowerDefencePawnBlueprint.GetDefaultObject()->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
    FVector SpawnLocation = SpawnTransform.GetLocation() + FVector(0.0f, 0.0f, HalfHeight);
    FHitResult Hit;
    const FVector TraceStart = SpawnLocation + FVector(0.0f, 0.0f, HalfHeight + 3000.0f);
    const FVector TraceEnd = SpawnLocation - FVector(0.0f, 0.0f, HalfHeight + 3000.0f);
    if (GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_WorldStatic))
        SpawnLocation.Z = Hit.Location.Z + HalfHeight;
    else
        TD_LOG_ERROR(
            TEXT(
                "UTowerDefencePawnFactory::CreatePawn -> spawn height trace missed ground entirely " "near %s, falling back to analytic height (may spawn floating or buried)"),
            *SpawnLocation.ToString());
    const FTransform SpawnLocationAndRotation(SpawnTransform.GetRotation(), SpawnLocation);
    auto& AvailablePool = AvailablePawnPools[TowerDefencePawnBlueprint];
    const auto* PawnPtr = AvailablePool.FindArbitraryElement();
    ATowerDefencePawn* Pawn;
    if (PawnPtr)
    {
        Pawn = PawnPtr->Get();
        AvailablePool.Remove(*PawnPtr);
        Pawn->SetActorTransform(SpawnLocationAndRotation, false, nullptr, ETeleportType::ResetPhysics);
        if (UCharacterMovementComponent* Movement = Pawn->GetCharacterMovement())
        {
            Movement->StopMovementImmediately();
            Movement->SetMovementMode(MOVE_Walking);
        }
    }
    else
    {
        FActorSpawnParameters SpawnParams;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
        Pawn =
            GetWorld()->SpawnActor<ATowerDefencePawn>(TowerDefencePawnBlueprint, SpawnLocationAndRotation, SpawnParams);
    }
    if (Pawn) UnavailablePawnPools[TowerDefencePawnBlueprint].Emplace(Pawn);
    return Pawn;
}

void UTowerDefencePawnFactory::DestroyPawn(ATowerDefencePawn* TowerDefencePawn)
{
    if (!IsValid(TowerDefencePawn)) return;
    if (!UnavailablePawnPools.Contains(TowerDefencePawn->GetClass())) return;
    const TStrongObjectPtr<ATowerDefencePawn> StrongPawn(TowerDefencePawn);
    auto& UnavailablePool = UnavailablePawnPools[TowerDefencePawn->GetClass()];
    if (!UnavailablePool.Contains(StrongPawn)) return;
    AvailablePawnPools[TowerDefencePawn->GetClass()].Add(StrongPawn);
    UnavailablePool.Remove(StrongPawn);
}

void UTowerDefencePawnFactory::CreatePool(const TSubclassOf<ATowerDefencePawn>& TowerDefencePawnBlueprint)
{
    AvailablePawnPools.Add(TowerDefencePawnBlueprint);
    AvailablePawnPools[TowerDefencePawnBlueprint].Reserve(StartingPoolSize);
    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    for (int32 i = 0; i < StartingPoolSize; ++i)
    {
        if (ATowerDefencePawn* Pawn =
                GetWorld()->SpawnActor<ATowerDefencePawn>(TowerDefencePawnBlueprint, FTransform::Identity, SpawnParams))
            AvailablePawnPools[TowerDefencePawnBlueprint].Emplace(Pawn);
    }
    UnavailablePawnPools.Add(TowerDefencePawnBlueprint);
    UnavailablePawnPools[TowerDefencePawnBlueprint].Reserve(StartingPoolSize);
}
