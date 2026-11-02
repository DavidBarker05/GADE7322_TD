#include "TowerDefencePawns/TowerDefencePawnBlueprintFunctionLibrary.h"

#include "TowerDefencePawns/Components/HealthComponent.h"

bool UTowerDefencePawnBlueprintFunctionLibrary::IsPawnActiveAndAlive(const ATowerDefencePawn* TowerDefencePawn)
{
    return IsValid(TowerDefencePawn) && TowerDefencePawn->IsPawnActive() &&
           TowerDefencePawn->GetHealthComponent()->IsAlive();
}

bool UTowerDefencePawnBlueprintFunctionLibrary::IsPawnGroundTroop(const ATowerDefencePawn* TowerDefencePawn)
{
    if (!IsValid(TowerDefencePawn) || !TowerDefencePawn->IsPawnActive() ||
        TowerDefencePawn->GetHealthComponent()->IsDead())
        return false;
    switch (TowerDefencePawn->GetCurrentTeam())
    {
        case EAITeam::FlyingAttacker:
            return true;
        default:
            return false;
    }
}
