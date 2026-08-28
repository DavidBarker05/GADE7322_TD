#include "TowerDefensePawns/Attackers/Skeleton/SkeletonAIController.h"

#include "Components/StateTreeAIComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AIPerceptionTypes.h"
#include "Perception/AISenseConfig_Sight.h"
#include "PlayerTower.h"
#include "TowerDefensePawns/Attackers/Skeleton/SkeletonPawn.h"

ASkeletonAIController::ASkeletonAIController()
{
    PrimaryActorTick.bCanEverTick = true;
    PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception Component"));
    SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));
    SightConfig->SightRadius = 500.0f;
    SightConfig->LoseSightRadius = 750.f;
    SightConfig->PeripheralVisionAngleDegrees = 180.0f;
    SightConfig->SetMaxAge(0.0f);
    SightConfig->DetectionByAffiliation.bDetectEnemies = true;
    SightConfig->DetectionByAffiliation.bDetectNeutrals = false;
    SightConfig->DetectionByAffiliation.bDetectFriendlies = false;
    PerceptionComponent->ConfigureSense(*SightConfig);
    PerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());
    StateTree = CreateDefaultSubobject<UStateTreeAIComponent>(TEXT("State Tree"));
    PerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &ASkeletonAIController::OnTargetPerceptionUpdated);
    PerceptionComponent->SetActive(false);
    StateTree->SetActive(false);
}

void ASkeletonAIController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    if (ASkeletonPawn* SKPawn = GetSkeletonPawn())
    {
        if (!SKPawn->IsPawnActive()) return;
        if (IsValid(SKPawn->GetAttackTarget()) &&
            (SKPawn->GetAttackTarget()->IsA<APlayerTower>() ||
             FVector::Dist2D(SKPawn->GetActorLocation(), SKPawn->GetAttackTarget()->GetActorLocation()) <=
                 SKPawn->GetAttackRadius() + KINDA_SMALL_NUMBER))
            return;
        if (TimeSinceLastVisionUpdate < 1.0f / VisionUpdateFrequency + KINDA_SMALL_NUMBER)
        {
            TimeSinceLastVisionUpdate += DeltaTime;
            return;
        }
        TimeSinceLastVisionUpdate = 0.0f;
        const ATowerDefensePawn* Closest = nullptr;
        for (int32 i = VisibleEnemies.Num() - 1; i >= 0; --i)
        {
            if (!IsValid(VisibleEnemies[i]) || !VisibleEnemies[i]->IsPawnActive())
            {
                VisibleEnemies.RemoveAt(i);
                continue;
            }
            if (!Closest ||
                FVector::DistSquared2D(SKPawn->GetActorLocation(), Closest->GetActorLocation()) >
                    FVector::DistSquared2D(SKPawn->GetActorLocation(), VisibleEnemies[i]->GetActorLocation()))
                Closest = VisibleEnemies[i];
        }
        SKPawn->SetAttackTarget(Closest);
    }
}

ASkeletonPawn* ASkeletonAIController::GetSkeletonPawn() const { return Cast<ASkeletonPawn>(GetPawn()); }

void ASkeletonAIController::SetControllerActive(bool bActive)
{
    if (!bActive)
    {
        PerceptionComponent->ForgetAll();
        VisibleEnemies.Empty();
    }
    PerceptionComponent->SetActive(bActive);
    StateTree->SetActive(bActive);
    TimeSinceLastVisionUpdate = 0.0f;
}

void ASkeletonAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
    if (ATowerDefensePawn* TDPawn = Cast<ATowerDefensePawn>(Actor))
    {
        if (!TDPawn->IsPawnActive()) return;
        if (Stimulus.WasSuccessfullySensed()) VisibleEnemies.Emplace(TDPawn);
        else VisibleEnemies.Remove(TDPawn);
    }
}
