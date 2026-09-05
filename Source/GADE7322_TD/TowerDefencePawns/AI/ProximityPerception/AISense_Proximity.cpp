#include "TowerDefencePawns/AI/ProximityPerception/AISense_Proximity.h"

#include "CustomLog.h"
#include "GameFramework/Actor.h"
#include "GenericTeamAgentInterface.h"
#include "Perception/AIPerceptionComponent.h"
#include "TowerDefencePawns/AI/ProximityPerception/AISenseConfig_Proximity.h"

UAISense_Proximity::UAISense_Proximity(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    if (!HasAnyFlags(RF_ClassDefaultObject))
        GetMutableDefault<UAISenseConfig_Proximity>()->Implementation = StaticClass();
    NotifyType = EAISenseNotifyType::OnPerceptionChange;
    // UAIPerceptionSystem::OnListenerForgetsAll gates the call to OnListenerForgetsAll() behind this flag
    // (defaults to false) - without it, ForgetAll() (called on SetControllerActive(false)) never actually
    // reaches our override below, silently leaving PairsInRange stale forever
    bNeedsForgettingNotification = true;
}

void UAISense_Proximity::RegisterSource(AActor& SourceActor)
{
    RegisteredSources.AddUnique(&SourceActor);
    TD_LOG_INFO(TEXT("UAISense_Proximity::RegisterSource -> %s, RegisteredSources.Num() = %d"),
               *SourceActor.GetName(), RegisteredSources.Num());
    // TimeUntilNextUpdate starts at SuspendNextUpdate (FLT_MAX) and nothing else in the base
    // UAISense ever lowers it - every built-in sense kicks its own update loop off via this call,
    // and without it ProgressTime() always returns false so Update() is never invoked
    RequestImmediateUpdate();
}

void UAISense_Proximity::UnregisterSource(AActor& SourceActor)
{
    RegisteredSources.RemoveSingleSwap(TWeakObjectPtr<AActor>(&SourceActor));
    TD_LOG_INFO(TEXT("UAISense_Proximity::UnregisterSource -> %s, RegisteredSources.Num() = %d"),
               *SourceActor.GetName(), RegisteredSources.Num());
}

void UAISense_Proximity::OnListenerForgetsAll(const FPerceptionListener& Listener)
{
    const FPerceptionListenerID ListenerID = Listener.GetListenerID();
    int32 RemovedCount = 0;
    for (auto It = PairsInRange.CreateIterator(); It; ++It)
    {
        if (It->Key != ListenerID) continue;
        It.RemoveCurrent();
        ++RemovedCount;
    }
    if (RemovedCount > 0)
        TD_LOG_INFO(TEXT("UAISense_Proximity::OnListenerForgetsAll -> %s, purged %d stale PairsInRange entries"),
                   Listener.Listener.IsValid() && Listener.Listener->GetOwner()
                       ? *Listener.Listener->GetOwner()->GetName()
                       : TEXT("INVALID"),
                   RemovedCount);
}

float UAISense_Proximity::Update()
{
    AIPerception::FListenerMap& ListenersMap = *GetListeners();
    // A listener whose controller deactivates (SetActive(false)) drops out of ListenersMap entirely, so
    // this loop never gets a chance to notice its sources "left range" and clean up PairsInRange. The
    // controller's own VisiblePawns DOES get reset on deactivation (a separate, correct mechanism), but
    // when the listener reactivates, any source it's still standing next to is already in PairsInRange
    // from before - treated as "no change, already notified" - so it never gets a fresh notification to
    // repopulate VisiblePawns, leaving it permanently blind to that source unless it leaves and re-enters
    // range. Purge entries for listeners that aren't currently present so they start fresh
    for (auto It = PairsInRange.CreateIterator(); It; ++It)
        if (!ListenersMap.Contains(It->Key)) It.RemoveCurrent();
    TSet<TPair<FPerceptionListenerID, TWeakObjectPtr<AActor>>> NewPairsInRange;
    TD_LOG_INFO(TEXT("UAISense_Proximity::Update -> Listeners.Num() = %d, RegisteredSources.Num() = %d"),
               ListenersMap.Num(), RegisteredSources.Num());
    for (const auto& ListenerEntry : ListenersMap)
    {
        const FPerceptionListener& Listener = ListenerEntry.Value;
        if (!Listener.HasSense(GetSenseID()))
        {
            TD_LOG_INFO(TEXT("UAISense_Proximity::Update -> Listener %s does not have this sense"),
                       Listener.Listener.IsValid() ? *Listener.Listener->GetOwner()->GetName() : TEXT("INVALID"));
            continue;
        }
        const UAIPerceptionComponent* ListenerComponent = Listener.Listener.Get();
        if (!ListenerComponent) continue;
        const UAISenseConfig_Proximity* SenseConfig =
            Cast<UAISenseConfig_Proximity>(ListenerComponent->GetSenseConfig(GetSenseID()));
        if (!SenseConfig)
        {
            TD_LOG_INFO(TEXT("UAISense_Proximity::Update -> Listener %s has no UAISenseConfig_Proximity"),
                       *ListenerComponent->GetOwner()->GetName());
            continue;
        }
        const float RadiusSq = FMath::Square(SenseConfig->DetectionRadius);
        const uint8 AffiliationFlags = SenseConfig->DetectionByAffiliation.GetAsFlags();
        const IGenericTeamAgentInterface* TeamAgent = Listener.GetTeamAgent();
        for (const TWeakObjectPtr<AActor>& SourceWeak : RegisteredSources)
        {
            const AActor* Source = SourceWeak.Get();
            if (!IsValid(Source) || Source == ListenerComponent->GetOwner()) continue;
            const bool bShouldSense = FAISenseAffiliationFilter::ShouldSenseTeam(TeamAgent, *Source, AffiliationFlags);
            const float DistSq = FVector::DistSquared(Source->GetActorLocation(), Listener.CachedLocation);
            TD_LOG_INFO(TEXT("UAISense_Proximity::Update -> Listener %s vs Source %s: TeamAgent = %d, bShouldSense = %d, Dist = %.0f, Radius = %.0f"),
                       *ListenerComponent->GetOwner()->GetName(), *Source->GetName(), TeamAgent != nullptr,
                       bShouldSense, FMath::Sqrt(DistSq), SenseConfig->DetectionRadius);
            if (!bShouldSense) continue;
            if (DistSq <= RadiusSq)
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
