#include "TowerDefensePawns/ProximityPerception/AISense_Proximity.h"

#include "GameFramework/Actor.h"
#include "GenericTeamAgentInterface.h"
#include "Perception/AIPerceptionComponent.h"
#include "TowerDefensePawns/ProximityPerception/AISenseConfig_Proximity.h"

UAISense_Proximity::UAISense_Proximity(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    if (!HasAnyFlags(RF_ClassDefaultObject))
        GetMutableDefault<UAISenseConfig_Proximity>()->Implementation = StaticClass();
    NotifyType = EAISenseNotifyType::OnPerceptionChange;
}

void UAISense_Proximity::RegisterSource(AActor& SourceActor) { RegisteredSources.AddUnique(&SourceActor); }

void UAISense_Proximity::UnregisterSource(AActor& SourceActor)
{
    RegisteredSources.RemoveSingleSwap(TWeakObjectPtr<AActor>(&SourceActor));
}

float UAISense_Proximity::Update()
{
    AIPerception::FListenerMap& ListenersMap = *GetListeners();
    TSet<TPair<FPerceptionListenerID, TWeakObjectPtr<AActor>>> NewPairsInRange;
    for (const auto& ListenerEntry : ListenersMap)
    {
        const FPerceptionListener& Listener = ListenerEntry.Value;
        if (!Listener.HasSense(GetSenseID())) continue;
        const UAIPerceptionComponent* ListenerComponent = Listener.Listener.Get();
        if (!ListenerComponent) continue;
        const UAISenseConfig_Proximity* SenseConfig =
            Cast<UAISenseConfig_Proximity>(ListenerComponent->GetSenseConfig(GetSenseID()));
        if (!SenseConfig) continue;
        const float RadiusSq = FMath::Square(SenseConfig->DetectionRadius);
        const uint8 AffiliationFlags = SenseConfig->DetectionByAffiliation.GetAsFlags();
        const IGenericTeamAgentInterface* TeamAgent = Listener.GetTeamAgent();
        for (const TWeakObjectPtr<AActor>& SourceWeak : RegisteredSources)
        {
            const AActor* Source = SourceWeak.Get();
            if (!IsValid(Source) || Source == ListenerComponent->GetOwner()) continue;
            if (!FAISenseAffiliationFilter::ShouldSenseTeam(TeamAgent, *Source, AffiliationFlags)) continue;
            if (FVector::DistSquared(Source->GetActorLocation(), Listener.CachedLocation) <= RadiusSq)
                NewPairsInRange.Add(
                    TPair<FPerceptionListenerID, TWeakObjectPtr<AActor>>(Listener.GetListenerID(), SourceWeak));
        }
    }
    for (const auto& Pair : NewPairsInRange)
    {
        if (PairsInRange.Contains(Pair)) continue;
        if (FPerceptionListener* Listener = ListenersMap.Find(Pair.Key))
        {
            if (AActor* Source = Pair.Value.Get())
                Listener->RegisterStimulus(Source,
                                           FAIStimulus(*this, 1.0f, Source->GetActorLocation(),
                                                       Listener->CachedLocation, FAIStimulus::SensingSucceeded));
        }
    }
    for (const auto& Pair : PairsInRange)
    {
        if (NewPairsInRange.Contains(Pair)) continue;
        if (FPerceptionListener* Listener = ListenersMap.Find(Pair.Key))
        {
            if (AActor* Source = Pair.Value.Get())
                Listener->RegisterStimulus(Source, FAIStimulus(*this, 0.0f, Source->GetActorLocation(),
                                                               Listener->CachedLocation, FAIStimulus::SensingFailed));
        }
    }
    PairsInRange = MoveTemp(NewPairsInRange);
    return 0.2f;
}
