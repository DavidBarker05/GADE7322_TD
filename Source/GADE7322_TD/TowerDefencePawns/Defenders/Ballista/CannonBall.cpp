#include "TowerDefencePawns/Defenders/Ballista/CannonBall.h"

#include "Kismet/KismetSystemLibrary.h"
#include "TowerDefencePawns/Attackers/Attacker.h"
#include "TowerDefencePawns/Components/HealthComponent.h"

void ACannonBall::FireProjectile(USceneComponent* InTarget, const FVector& LaunchVelocity, int32 InDamage,
                                 const TArray<FAny>& LaunchArguments)
{
    if (!IsValid(InTarget) || InDamage <= 0) return;
    if (LaunchArguments.Num() != 2 && !LaunchArguments[0].Get<float>() && !LaunchArguments[1].Get<float>()) return;
    GroundTroopDamageReduction = *LaunchArguments[0].Get<float>();
    ExplosionRadius = *LaunchArguments[1].Get<float>();
    FireWithoutLaunch(InTarget, InDamage);
    Launch(LaunchVelocity);
}

void ACannonBall::DamageTarget(ATowerDefencePawn* TargetToDamage)
{
    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes = {UEngineTypes::ConvertToObjectType(ECC_Pawn)};
    TArray<AActor*> OutActors;
    if (UKismetSystemLibrary::SphereOverlapActors(this, GetActorLocation(), ExplosionRadius, ObjectTypes,
                                                  AAttacker::StaticClass(), {}, OutActors))
    {
        const int32 AirDamage = GetDamage();
        const int32 GroundDamage = FMath::RoundToInt(GetDamage() * (1.0f - GroundTroopDamageReduction / 100.0f));
        for (AActor* Actor : OutActors)
        {
            AAttacker* Attacker = Cast<AAttacker>(Actor);
            if (!ATowerDefencePawn::IsPawnActiveAndAlive(Attacker)) continue;
            const int32 FinalDamage = ATowerDefencePawn::IsPawnGroundTroop(Attacker) ? GroundDamage : AirDamage;
            Attacker->GetHealthComponent()->TakeDamage(FinalDamage);
        }
    }
}
