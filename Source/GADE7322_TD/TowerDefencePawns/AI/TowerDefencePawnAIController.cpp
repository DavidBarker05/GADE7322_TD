// ReSharper disable CppParameterMayBeConst
#include "TowerDefencePawns/AI/TowerDefencePawnAIController.h"

#include "Components/StateTreeAIComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AIPerceptionTypes.h"
#include "TowerDefencePawn.h"
#include "TowerDefencePawns/AI/ProximityPerception/AISenseConfig_Proximity.h"
#include "TowerDefencePawns/Components/HealthComponent.h"

ATowerDefencePawnAIController::ATowerDefencePawnAIController()
{
    PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception Component"));
    ProximityConfig = CreateDefaultSubobject<UAISenseConfig_Proximity>(TEXT("Proximity Config"));
    PerceptionComponent->ConfigureSense(*ProximityConfig);
    PerceptionComponent->SetDominantSense(ProximityConfig->GetSenseImplementation());
    StateTree = CreateDefaultSubobject<UStateTreeAIComponent>(TEXT("State Tree"));
    PerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(
        this, &ATowerDefencePawnAIController::OnTargetPerceptionUpdated);
}
void ATowerDefencePawnAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);
    SetControllerActive(true);
}

void ATowerDefencePawnAIController::OnUnPossess()
{
    SetControllerActive(false);
    Super::OnUnPossess();
}

void ATowerDefencePawnAIController::SetControllerActive(bool bActive)
{
    if (!bActive)
    {
        PerceptionComponent->ForgetAll();
        VisiblePawns.Empty();
    }
    PerceptionComponent->SetActive(bActive);
    StateTree->SetActive(bActive);
}

const ATowerDefencePawn* ATowerDefencePawnAIController::GetTowerDefensePawn() const
{
    return Cast<ATowerDefencePawn>(GetPawn());
}

void ATowerDefencePawnAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
    if (!IsValid(Actor)) return;
    if (ATowerDefencePawn* TDPawn = Cast<ATowerDefencePawn>(Actor))
    {
        if (!Stimulus.WasSuccessfullySensed()) VisiblePawns.Remove(TDPawn);
        else if (TDPawn->IsPawnActive() && TDPawn->GetHealthComponent()->IsAlive()) VisiblePawns.AddUnique(TDPawn);
    }
}
