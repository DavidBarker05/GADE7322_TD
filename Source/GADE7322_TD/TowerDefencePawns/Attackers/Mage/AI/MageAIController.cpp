#include "TowerDefencePawns/Attackers/Mage/AI/MageAIController.h"

#include "HealthComponent.h"
#include "Perception/AIPerceptionTypes.h"
#include "TowerDefenceGameMode.h"
#include "TowerDefencePawns/AI/ProximityPerception/AISenseConfig_Proximity.h"
#include "TowerDefencePawns/AI/TargetSelectionFunctions.h"
#include "TowerDefencePawns/Attackers/Mage/Mage.h"
#include "TowerDefencePawns/Defenders/Ballista/Ballista.h"
#include "TowerDefencePawns/Defenders/Healer/Healer.h"
#include "TowerDefencePawns/Defenders/Warrior/Warrior.h"
#include "TowerDefencePawns/Tower/PlayerTower.h"

AMageAIController::AMageAIController()
{
    PrimaryActorTick.bCanEverTick = true;
    if (const auto ProxConfig = GetProximityConfig())
    {
        ProxConfig->DetectionRadius = 1000.0f;
        ProxConfig->DetectionByAffiliation.bDetectEnemies = true; // Attacking as a last resort
        ProxConfig->DetectionByAffiliation.bDetectNeutrals = false;
        ProxConfig->DetectionByAffiliation.bDetectFriendlies = true; // Boost
    }
}

void AMageAIController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    AMage* Mage = GetMage();
    if (!IsValid(Mage) || !Mage->IsPawnActive()) return;

    // The logic is a bit tricky for this, so I had to map it out to make my life easier
    // here is that logic:
    //
    // Is there a current target?
    // | -> Yes: Is the current target in the radius?
    // | | -> Yes: Keep using that target
    // | | -> No: Are there any defenders in vision?
    // | . | -> Yes: Are there any defenders in attack radius?
    // | . | | -> Yes: Are any of those defenders targeting this pawn?
    // | . | | | -> Yes: Attack the closest of those defenders
    // | . | | | -> No: Are there any attackers in vision?
    // | . | | | | -> Yes: Are any of those attackers in boost radius?
    // | . | | . | | -> Yes: Boost the closest of those attackers
    // | . | | . | | -> No: Attack the closest of those defenders
    // | . | | . | -> No: Attack the closest of those defenders
    // | . | | -> No: Are there any attackers in vision?
    // | . | . | -> Yes: Are there any attackers in boost radius?
    // | . | . | | -> Yes: Boost the closest of those attackers
    // | . | . | | -> No: Head to the closest of those attackers
    // | . | . | -> No: Head to the closest of those attackers
    // | . | -> No: Are there any attackers in vision?
    // | . . | -> Yes: Are there any attackers in boost radius?
    // | . . | | -> Yes: Boost the closest of those attackers
    // | . . | | -> No: Head to the closest of those attackers
    // | . . | -> No: Head towards the tower
    // | -> No: Are there any defenders in vision?
    // . | -> Yes: Are there any defenders in attack radius?
    // . | | -> Yes: Are any of those defenders targeting this pawn?
    // . | | | -> Yes: Attack the closest of those defenders
    // . | | | -> No: Are there any attackers in vision?
    // . | | | | -> Yes: Are any of those attackers in boost radius?
    // . | | . | | -> Yes: Boost the closest of those attackers
    // . | | . | | -> No: Attack the closest of those defenders
    // . | | . | -> No: Attack the closest of those defenders
    // . | | -> No: Are there any attackers in vision?
    // . | . | -> Yes: Are there any attackers in boost radius?
    // . | . | | -> Yes: Boost the closest of those attackers
    // . | . | | -> No: Head to the closest of those attackers
    // . | . | -> No: Head to the closest of those attackers
    // . | -> No: Are there any attackers in vision?
    // . . | -> Yes: Are there any attackers in boost radius?
    // . . | | -> Yes: Boost the closest of those attackers
    // . . | | -> No: Head to the closest of those attackers
    // . . | -> No: Head towards the tower

    if (const ATowerDefencePawn* Target = Mage->GetCurrentTarget();
        IsValid(Target) && Target->IsPawnActive() &&
        Target->GetHealthComponent()->IsAlive()) // Is there a current target?
    {
        const float Radius = IsOtherPawnFriendly(Target) ? Mage->GetBoostRadius() : Mage->GetAttackRadius();
        if (FVector::Dist2D(Mage->GetActorLocation(), Target->GetActorLocation()) <=
            Radius + KINDA_SMALL_NUMBER) // Is the current target in the radius?
        {
            if (const FVector ToTarget = Target->GetActorLocation() - Mage->GetActorLocation();
                !ToTarget.IsNearlyZero())
                Mage->SetActorRotation(FRotator(0.0f, ToTarget.Rotation().Yaw, 0.0f));
            return; // Keep using that target
        }
    }
    if (TimeSinceLastVisionUpdate < 1.0f / VisionUpdateFrequency + KINDA_SMALL_NUMBER)
    {
        TimeSinceLastVisionUpdate += DeltaTime;
        return;
    }
    TimeSinceLastVisionUpdate = 0.0f;
    for (int32 i = GetVisiblePawns().Num() - 1; i >= 0; --i)
    {
        if (const ATowerDefencePawn* TDPawn = GetVisiblePawns()[i])
        {
            if (!IsValid(TDPawn) || !TDPawn->IsPawnActive() || TDPawn->GetHealthComponent()->IsDead())
                GetVisiblePawns().RemoveAt(i);
        }
        else GetVisiblePawns().RemoveAt(i);
    }
    FVector MageLoc = Mage->GetActorLocation();
    auto GetCurrentVal = [this, &MageLoc](const ATowerDefencePawn* Other) -> TPair<float, bool>
    {
        float Dist = FVector::Dist2D(MageLoc, Other->GetActorLocation()) - Other->GetOccupiedRadius();
        bool bIsTargetForOther = IsThisATargetForOtherPawn(Other);
        return TPair<float, bool>(Dist, bIsTargetForOther);
    };
    auto Predicate = [](const TPair<float, bool>& Left, const TPair<float, bool>& Right) -> bool
    {
        if (!Left.Value && Right.Value) return true; // Not target for current, but is target for other
        return Left.Key < Right.Key; // Closest in all other cases
    };
    ATowerDefencePawn* KindaClosestEnemy = SelectTarget<TPair<float, bool>>(
        VisibleEnemies, TPair<float, bool>(TNumericLimits<float>::Max(), false), GetCurrentVal, Predicate);
    ATowerDefencePawn* ClosestFriendly = SelectClosestTarget(VisibleFriendlies, Mage);
    if (KindaClosestEnemy) // Are there any defenders in vision?
    {
        float DistToEnemy =
            FVector::Dist2D(MageLoc, KindaClosestEnemy->GetActorLocation()) - KindaClosestEnemy->GetOccupiedRadius();
        bool bIsTargetForEnemy = IsThisATargetForOtherPawn(KindaClosestEnemy);
        if (DistToEnemy <= Mage->GetAttackRadius() + KINDA_SMALL_NUMBER) // Are there any defenders in attack radius?
        {
            if (bIsTargetForEnemy) // Are any of those defenders targeting this pawn?
                Mage->SetCurrentTarget(KindaClosestEnemy); // Attack the closest of those defenders
            else if (ClosestFriendly) // Are there any attackers in vision?
            {
                float DistToFriendly = FVector::Dist2D(MageLoc, ClosestFriendly->GetActorLocation()) -
                                       ClosestFriendly->GetOccupiedRadius();
                if (DistToFriendly <=
                    Mage->GetBoostRadius() + KINDA_SMALL_NUMBER) // Are any of those attackers in boost radius?
                    Mage->SetCurrentTarget(ClosestFriendly); // Boost the closest of those defenders
                else Mage->SetCurrentTarget(KindaClosestEnemy); // Attack the closest of those defenders
            }
            else Mage->SetCurrentTarget(KindaClosestEnemy); // Attack the closest of those defenders
        }
        else if (ClosestFriendly) // Are there any attackers in vision?
        {
            Mage->SetCurrentTarget(ClosestFriendly);
            // ^ Does both
            // Are there any attackers in boost radius?
            // | -> Yes: Boost the closest of those attackers
            // | -> No: Head to the closest of those attackers
        }
        else Mage->SetCurrentTarget(KindaClosestEnemy); // Head to the closest of those defenders
    }
    else if (ClosestFriendly) // Are there any attackers in vision?
    {
        Mage->SetCurrentTarget(ClosestFriendly);
        // ^ Does both
        // Are there any attackers in boost radius?
        // | -> Yes: Boost the closest of those attackers
        // | -> No: Head to the closest of those attackers
    }
    else Mage->SetCurrentTarget(nullptr); // Head towards the tower
    // I think that is correct for my initial plan?
    // It seems right...
}

void AMageAIController::SetControllerActive(bool bActive)
{
    Super::SetControllerActive(bActive);
    TimeSinceLastVisionUpdate = 0.0f;
}

void AMageAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
    if (!IsValid(Actor)) return;
    if (ATowerDefencePawn* TDPawn = Cast<ATowerDefencePawn>(Actor))
    {
        if (!Stimulus.WasSuccessfullySensed())
        {
            GetVisiblePawns().Remove(TDPawn);
            if (IsOtherPawnFriendly(TDPawn)) VisibleFriendlies.Remove(TDPawn);
            else VisibleEnemies.Remove(TDPawn);
        }
        else if (TDPawn->IsPawnActive() && TDPawn->GetHealthComponent()->IsAlive())
        {
            GetVisiblePawns().AddUnique(TDPawn);
            if (IsOtherPawnFriendly(TDPawn)) VisibleFriendlies.AddUnique(TDPawn);
            else VisibleEnemies.AddUnique(TDPawn);
        }
    }
}

AMage* AMageAIController::GetMage() const { return GetPawn<AMage>(); }

bool AMageAIController::IsOtherPawnFriendly(const ATowerDefencePawn* OtherPawn) const
{
    if (const AMage* Mage = GetMage(); IsValid(OtherPawn) && OtherPawn->IsPawnActive())
    {
        const ETeamAttitude::Type Attitude =
            ATowerDefenceGameMode::GetAttitudeCustom(Mage->GetCurrentTeam(), OtherPawn->GetCurrentTeam());
        return Attitude == ETeamAttitude::Friendly;
    }
    return false;
}

bool AMageAIController::IsThisATargetForOtherPawn(const ATowerDefencePawn* OtherPawn) const
{
    if (!IsValid(OtherPawn) || !OtherPawn->IsPawnActive() || OtherPawn->GetHealthComponent()->IsDead()) return false;
    const AMage* Mage = GetMage();
    if (!Mage) return false;
    // I hate doing it like this, but ATowerDefencePawn doesn't have a current target
    // (because some things like tower have multiple) so there is no better way to do this
    if (const AWarrior* Warrior = Cast<AWarrior>(OtherPawn)) return Warrior->GetAttackTarget() == Mage;
    if (const ABallista* Ballista = Cast<ABallista>(OtherPawn)) return Ballista->GetAttackTarget() == Mage;
    if (const AHealer* Healer = Cast<AHealer>(OtherPawn)) return Healer->GetCurrentTarget() == Mage;
    if (const APlayerTower* Tower = Cast<APlayerTower>(OtherPawn)) return Tower->GetAttackTargets().Contains(Mage);
    return false;
}
