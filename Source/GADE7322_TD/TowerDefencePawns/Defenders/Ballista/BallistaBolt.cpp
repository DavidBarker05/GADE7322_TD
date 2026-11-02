#include "TowerDefencePawns/Defenders/Ballista/BallistaBolt.h"

#include "TowerDefencePawns/Components/HealthComponent.h"
#include "TowerDefencePawns/TowerDefencePawn.h"

void ABallistaBolt::FireProjectile(USceneComponent* InTarget, const FVector& LaunchVelocity, int32 InDamage,
                                   const TArray<FAny>& LaunchArguments)
{
    if (!IsValid(InTarget) || InDamage <= 0) return;
    if (LaunchArguments.Num() != 1 && LaunchArguments.Num() != 2) return;
    if (!LaunchArguments[0].Get<bool>()) return;
    bDamageGroundTroops = *LaunchArguments[0].Get<bool>();
    if (bDamageGroundTroops)
    {
        if (LaunchArguments.Num() != 2 && !LaunchArguments[1].Get<float>()) return;
        GroundTroopDamageReduction = *LaunchArguments[1].Get<float>();
    }
    FireWithoutLaunch(InTarget, InDamage);
    Launch(LaunchVelocity);
}

void ABallistaBolt::DamageTarget(ATowerDefencePawn* TargetToDamage)
{
    if (!ATowerDefencePawn::IsPawnActiveAndAlive(TargetToDamage)) return;
    const int32 FinalDamage = ATowerDefencePawn::IsPawnGroundTroop(TargetToDamage) ?
                                  FMath::RoundToInt(GetDamage() * (1.0f - GroundTroopDamageReduction / 100.0f)) :
                                  GetDamage();
    TargetToDamage->GetHealthComponent()->TakeDamage(FinalDamage);
}
