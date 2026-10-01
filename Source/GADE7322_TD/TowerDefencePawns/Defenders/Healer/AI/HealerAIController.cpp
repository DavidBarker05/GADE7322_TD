#include "TowerDefencePawns/Defenders/Healer/AI/HealerAIController.h"

#include "HealthComponent.h"
#include "Perception/AIPerceptionTypes.h"
#include "TowerDefenceGameMode.h"
#include "TowerDefencePawns/AI/ProximityPerception/AISenseConfig_Proximity.h"
#include "TowerDefencePawns/AI/TargetSelectionFunctions.h"
#include "TowerDefencePawns/Attackers/FlyingEnemy/FlyingEnemy.h"
#include "TowerDefencePawns/Attackers/Mage/Mage.h"
#include "TowerDefencePawns/Attackers/Skeleton/SkeletonPawn.h"
#include "TowerDefencePawns/Defenders/Healer/Healer.h"
#include "TowerDefencePawns/Tower/PlayerTower.h"

AHealerAIController::AHealerAIController()
{
    PrimaryActorTick.bCanEverTick = true;
    if (const auto ProxConfig = GetProximityConfig())
    {
        ProxConfig->DetectionRadius = 1200.0f;
        // ^ I know it's very far, but healers need to be able to see defenders
        // so that they can go to them to heal them, we don't really want to
        // have to place healers right next to attackers for them to do anything
        ProxConfig->DetectionByAffiliation.bDetectEnemies = true; // Attacking as a last resort
        ProxConfig->DetectionByAffiliation.bDetectNeutrals = false;
        ProxConfig->DetectionByAffiliation.bDetectFriendlies = true; // Healing
    }
}

void AHealerAIController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    AHealer* Healer = GetHealer();
    if (!IsValid(Healer) || !Healer->IsPawnActive()) return;

    // The logic is a bit tricky for this, so I had to map it out to make my life easier
    // here is that logic:
    //
    // Is there a current target?
    // | -> Yes: Is the current target in the radius?
    // | | -> Yes: Keep using that target
    // | | -> No: Are there any attackers in vision?
    // | . | -> Yes: Are there any attackers in attack radius?
    // | . | | -> Yes: Are any of those attackers targeting this pawn?
    // | . | | | -> Yes: Attack the closest of those attackers
    // | . | | | -> No: Are there any defenders in vision?
    // | . | | | | -> Yes: Are any of those defenders in heal radius?
    // | . | | . | | -> Yes: Heal the closest of those defenders
    // | . | | . | | -> No: Attack the closest of those attackers
    // | . | | . | -> No: Attack the closest of those attackers
    // | . | | -> No: Are there any defenders in vision?
    // | . | . | -> Yes: Are there any defenders in heal radius?
    // | . | . | | -> Yes: Heal the closest of those defenders
    // | . | . | | -> No: Head to the closest of those defenders
    // | . | . | -> No: Head to the closest of those attackers
    // | . | -> No: Are there any defenders in vision?
    // | . . | -> Yes: Are there any defenders in heal radius?
    // | . . | | -> Yes: Heal the closest of those defenders
    // | . . | | -> No: Head to the closest of those defenders
    // | . . | -> No: Head back to placement spot
    // | -> No: Are there any attackers in vision?
    // . | -> Yes: Are there any attackers in attack radius?
    // . | | -> Yes: Are any of those attackers targeting this pawn?
    // . | | | -> Yes: Attack the closest of those attackers
    // . | | | -> No: Are there any defenders in vision?
    // . | | | | -> Yes: Are any of those defenders in heal radius?
    // . | | . | | -> Yes: Heal the closest of those defenders
    // . | | . | | -> No: Attack the closest of those attackers
    // . | | . | -> No: Attack the closest of those attackers
    // . | | -> No: Are there any defenders in vision?
    // . | . | -> Yes: Are there any defenders in heal radius?
    // . | . | | -> Yes: Heal the closest of those defenders
    // . | . | | -> No: Head to the closest of those defenders
    // . | . | -> No: Head to the closest of those attackers
    // . | -> No: Are there any defenders in vision?
    // . . | -> Yes: Are there any defenders in heal radius?
    // . . | | -> Yes: Heal the closest of those defenders
    // . . | | -> No: Head to the closest of those defenders
    // . . | -> No: Head back to placement spot

    // Additional thing realised later and don't want to rewrite my comment:
    // Don't want healers to get in a circle of healing and therefore get stuck
    // healing each other and never be able to heal other defenders or attack attackers.
    // So need to not target a healer if they are healing someone else

    if (const ATowerDefencePawn* Target = Healer->GetCurrentTarget();
        IsValid(Target) && Target->IsPawnActive() &&
        Target->GetHealthComponent()->IsAlive()) // Is there a current target?
    {
        const float Radius = IsOtherPawnFriendly(Target) ? Healer->GetHealRadius() : Healer->GetAttackRadius();
        if (FVector::Dist2D(Healer->GetActorLocation(), Target->GetActorLocation()) <=
            Radius + KINDA_SMALL_NUMBER) // Is the current target in the radius?
        {
            if (const FVector ToTarget = Target->GetActorLocation() - Healer->GetActorLocation();
                !ToTarget.IsNearlyZero())
                Healer->SetActorRotation(FRotator(0.0f, ToTarget.Rotation().Yaw, 0.0f));
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
    FVector HealerLoc = Healer->GetActorLocation();
    auto Predicate = [](const TPair<float, bool>& Left, const TPair<float, bool>& Right) -> bool
    {
        if (Left.Value != Right.Value) return Left.Value;
        return Left.Key < Right.Key;
    };
    auto GetCurrentValEnemy = [this, &HealerLoc](const ATowerDefencePawn* Other) -> TPair<float, bool>
    {
        float Dist = FVector::Dist2D(HealerLoc, Other->GetActorLocation()) - Other->GetOccupiedRadius();
        bool bIsTargetForOther = IsThisATargetForOtherPawn(Other); // Prioritise targeting enemies targeting this pawn
        return TPair<float, bool>(Dist, bIsTargetForOther);
    };
    ATowerDefencePawn* KindaClosestEnemy = SelectTarget<TPair<float, bool>>(
        VisibleEnemies, TPair<float, bool>(TNumericLimits<float>::Max(), false), GetCurrentValEnemy, Predicate);
    auto IsOtherPawnHealing = [](const ATowerDefencePawn* Other) -> bool
    {
        if (const AHealer* OtherHealer = Cast<AHealer>(Other)) return OtherHealer->IsCurrentTargetFriendly();
        return false;
    };
    auto GetCurrentValFriendly = [this, &HealerLoc,
                                  &IsOtherPawnHealing](const ATowerDefencePawn* Other) -> TPair<float, bool>
    {
        float Dist = FVector::Dist2D(HealerLoc, Other->GetActorLocation()) - Other->GetOccupiedRadius();
        const bool bIsOtherPawnHealing =
            IsOtherPawnHealing(Other); // Don't target healers that are healing someone else
        return TPair<float, bool>(Dist, !bIsOtherPawnHealing);
    };
    ATowerDefencePawn* KindaClosestFriendly = SelectTarget<TPair<float, bool>>(
        VisibleFriendlies, TPair<float, bool>(TNumericLimits<float>::Max(), false), GetCurrentValFriendly, Predicate);
    if (KindaClosestEnemy) // Are there any attackers in vision?
    {
        float DistToEnemy =
            FVector::Dist2D(HealerLoc, KindaClosestEnemy->GetActorLocation()) - KindaClosestEnemy->GetOccupiedRadius();
        bool bIsTargetForEnemy = IsThisATargetForOtherPawn(KindaClosestEnemy);
        if (DistToEnemy <= Healer->GetAttackRadius() + KINDA_SMALL_NUMBER) // Are there any attackers in attack radius?
        {
            if (bIsTargetForEnemy) // Are any of those attackers targeting this pawn?
                Healer->SetCurrentTarget(KindaClosestEnemy); // Attack the closest of those attackers
            else if (KindaClosestFriendly &&
                     !IsOtherPawnHealing(KindaClosestFriendly)) // Are there any defenders in vision? (and the other
                                                                // defender can't be healing someone)
            {
                float DistToFriendly = FVector::Dist2D(HealerLoc, KindaClosestFriendly->GetActorLocation()) -
                                       KindaClosestFriendly->GetOccupiedRadius();
                if (DistToFriendly <=
                    Healer->GetHealRadius() + KINDA_SMALL_NUMBER) // Are any of those defenders in heal radius?
                    Healer->SetCurrentTarget(KindaClosestFriendly); // Heal the closest of those defenders
                else Healer->SetCurrentTarget(KindaClosestEnemy); // Attack the closest of those attackers
            }
            else Healer->SetCurrentTarget(KindaClosestEnemy); // Attack the closest of those attackers
        }
        else if (KindaClosestFriendly &&
                 !IsOtherPawnHealing(KindaClosestFriendly)) // Are there any defenders in vision? (and the other
                                                            // defender can't be healing someone)
        {
            Healer->SetCurrentTarget(KindaClosestFriendly);
            // ^ Does both
            // Are there any defenders in heal radius?
            // | -> Yes: Heal the closest of those defenders
            // | -> No: Head to the closest of those defenders
        }
        else Healer->SetCurrentTarget(KindaClosestEnemy); // Head to the closest of those attackers
    }
    else if (KindaClosestFriendly &&
             !IsOtherPawnHealing(KindaClosestFriendly)) // Are there any defenders in vision? (and the other defender
                                                        // can't be healing someone)
    {
        Healer->SetCurrentTarget(KindaClosestFriendly);
        // ^ Does both
        // Are there any defenders in heal radius?
        // | -> Yes: Heal the closest of those defenders
        // | -> No: Head to the closest of those defenders
    }
    else Healer->SetCurrentTarget(nullptr); // Head back to placement spot
    // I think that is correct for my initial plan?
    // It seems right...
}

void AHealerAIController::SetControllerActive(bool bActive)
{
    Super::SetControllerActive(bActive);
    TimeSinceLastVisionUpdate = 0.0f;
}

void AHealerAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
    if (!IsValid(Actor) || Actor == GetPawn()) return;
    if (ATowerDefencePawn* TDPawn = Cast<ATowerDefencePawn>(Actor))
    {
        if (TDPawn->IsA<APlayerTower>()) return; // Don't heal player tower
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

AHealer* AHealerAIController::GetHealer() const { return GetPawn<AHealer>(); }

bool AHealerAIController::IsOtherPawnFriendly(const ATowerDefencePawn* OtherPawn) const
{
    if (const AHealer* Healer = GetHealer(); IsValid(OtherPawn) && OtherPawn->IsPawnActive())
    {
        const ETeamAttitude::Type Attitude =
            ATowerDefenceGameMode::GetAttitudeCustom(Healer->GetCurrentTeam(), OtherPawn->GetCurrentTeam());
        return Attitude == ETeamAttitude::Friendly;
    }
    return false;
}

bool AHealerAIController::IsThisATargetForOtherPawn(const ATowerDefencePawn* OtherPawn) const
{
    if (!IsValid(OtherPawn) || !OtherPawn->IsPawnActive() || OtherPawn->GetHealthComponent()->IsDead()) return false;
    const AHealer* Healer = GetHealer();
    if (!Healer) return false;
    // I hate doing it like this, but ATowerDefencePawn doesn't have a current target
    // (because some things like tower have multiple) so there is no better way to do this
    if (const ASkeletonPawn* Skel = Cast<ASkeletonPawn>(OtherPawn)) return Skel->GetAttackTarget() == Healer;
    if (const AFlyingEnemy* Flyer = Cast<AFlyingEnemy>(OtherPawn)) return Flyer->GetAttackTarget() == Healer;
    if (const AMage* Mage = Cast<AMage>(OtherPawn)) return Mage->GetCurrentTarget() == Healer;
    return false;
}
