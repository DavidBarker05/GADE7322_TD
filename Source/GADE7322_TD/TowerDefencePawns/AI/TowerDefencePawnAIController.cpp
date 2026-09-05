// ReSharper disable CppParameterMayBeConst
#include "TowerDefencePawns/AI/TowerDefencePawnAIController.h"

#include "Components/StateTreeAIComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AIPerceptionTypes.h"
#include "TowerDefencePawns/AI/ProximityPerception/AISenseConfig_Proximity.h"
#include "TowerDefencePawns/Components/HealthComponent.h"
#include "TowerDefencePawns/TowerDefencePawn.h"

ATowerDefencePawnAIController::ATowerDefencePawnAIController()
{
    PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception Component"));
    ProximityConfig = CreateDefaultSubobject<UAISenseConfig_Proximity>(TEXT("Proximity Config"));
    PerceptionComponent->ConfigureSense(*ProximityConfig);
    PerceptionComponent->SetDominantSense(ProximityConfig->GetSenseImplementation());
    StateTree = CreateDefaultSubobject<UStateTreeAIComponent>(TEXT("State Tree"));
    PerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(
        this, &ATowerDefencePawnAIController::OnTargetPerceptionUpdated);
    if (UPathFollowingComponent* PFC = GetPathFollowingComponent()) PFC->SetBlockDetection(25.0f, 0.5f, 10);
}
void ATowerDefencePawnAIController::SetControllerActive(bool bActive)
{
    if (!bActive)
    {
        PerceptionComponent->ForgetAll();
        VisiblePawns.Empty();
    }
    PerceptionComponent->SetActive(bActive);
    if (bActive) StateTree->RestartLogic();
    else StateTree->StopLogic(TEXT("Controller deactivated"));
}

const ATowerDefencePawn* ATowerDefencePawnAIController::GetTowerDefensePawn() const
{
    return GetPawn<ATowerDefencePawn>();
}

FGenericTeamId ATowerDefencePawnAIController::GetGenericTeamId() const
{
    const ATowerDefencePawn* TDPawn = GetTowerDefensePawn();
    return TDPawn ? TDPawn->GetGenericTeamId() : FGenericTeamId::NoTeam;
}

const FNavAgentProperties& ATowerDefencePawnAIController::GetNavAgentPropertiesRef() const
{
    const ATowerDefencePawn* TDPawn = GetTowerDefensePawn();
    return TDPawn ? TDPawn->GetCharacterMovement()->GetNavAgentPropertiesRef() : NavAgentProps;
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
