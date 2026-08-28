#include "Events/EventBus.h"

#include "Events/EventListener.h"

void UEventBus::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    EventListeners.Empty();
}

void UEventBus::Deinitialize()
{
    EventListeners.Empty();
    Super::Deinitialize();
}

void UEventBus::AddListener(const FName& EventName, TScriptInterface<IEventListener> Listener)
{
    if (EventListeners.Contains(EventName)) EventListeners.Find(EventName)->AddUnique(Listener);
    else EventListeners.Add(EventName, {Listener});
}

void UEventBus::RemoveListener(const FName& EventName, TScriptInterface<IEventListener> Listener)
{
    if (TArray<TScriptInterface<IEventListener>>* Listeners = EventListeners.Find(EventName))
    {
        // Faster remove, because order doesn't really matter
        Listeners->RemoveSwap(Listener, EAllowShrinking::No);
        if (Listeners->IsEmpty()) EventListeners.Remove(EventName);
    }
}

void UEventBus::Broadcast(const FName& EventName, const TArray<FAny>& Params)
{
    if (TArray<TScriptInterface<IEventListener>>* Listeners = EventListeners.Find(EventName))
    {
        if (Listeners->IsEmpty()) return;
        for (int32 i = Listeners->Num() - 1; i >= 0; --i)
            if ((*Listeners)[i] && IsValid((*Listeners)[i].GetObject()))
                (*Listeners)[i]->Execute_OnEventReceived((*Listeners)[i].GetObject(), EventName, Params);
    }
}
