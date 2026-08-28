#pragma once

#include "CoreMinimal.h"

#include "Any/Any.h"
#include "Events/EventBus.h"
#include "UObject/Interface.h"

#include "EventListener.generated.h"

#ifndef EVENTS_TO_LISTEN_TO
// Write this in the header file
#define EVENTS_TO_LISTEN_TO(...) TArray<FString> EventNames = {__VA_ARGS__};
#endif

#ifndef SUBSCRIBE_TO_EVENTS
// Write this in the cpp file in the BeginPlay function
#define SUBSCRIBE_TO_EVENTS() \
    do \
    { \
        if (UEventBus* EventBus = Internal::GetEventBusFromContext(this)) \
        { \
            for (const FString& EventName : EventNames) EventBus->AddListener(FName(EventName), this); \
        } \
    } while (0)
#endif

#ifndef SUBSCRIBE_TO_EVENTS_RUNTIME
// Subscribe to new events during runtime, write this in the cpp file inside a function
#define SUBSCRIBE_TO_EVENTS_RUNTIME(...) \
    do \
    { \
        if (UEventBus* EventBus = Internal::GetEventBusFromContext(this)) \
        { \
            FString TempNames[] = {__VA_ARGS__}; \
            for (const FString& Name : TempNames) \
            { \
                if (EventNames.Contains(Name)) continue; \
                (void)EventNames.Add(Name); \
                EventBus->AddListener(FName(Name), this); \
            } \
        } \
    } while (0)
#endif

#ifndef UNSUBSCRIBE_FROM_EVENTS
// Write this in the cpp file in the EndPlay function
#define UNSUBSCRIBE_FROM_EVENTS() \
    do \
    { \
        if (UEventBus* EventBus = Internal::GetEventBusFromContext(this)) \
        { \
            for (const FString& EventName : EventNames) EventBus->RemoveListener(FName(EventName), this); \
        } \
    } while (0)
#endif

#ifndef UNSUBSCRIBE_FROM_EVENTS_RUNTIME
// Unsubscribe from events during runtime, write this in the cpp file inside a function
#define UNSUBSCRIBE_FROM_EVENTS_RUNTIME(...) \
    do \
    { \
        if (UEventBus* EventBus = Internal::GetEventBusFromContext(this)) \
        { \
            FString TempNames[] = {__VA_ARGS__}; \
            for (const FString& Name : TempNames) \
            { \
                if (!EventNames.Contains(Name)) continue; \
                (void)EventNames.Remove(Name); \
                EventBus->RemoveListener(FName(Name), this); \
            } \
        } \
    } while (0)
#endif

#ifndef EVENT_MATCHES
#define EVENT_MATCHES(Name, NumArgs) EventName.IsEqual(Name) && Params.Num() == NumArgs
#endif

namespace Internal
{
    static bool EventParamsAreValid(const TArray<FAny>& Params)
    {
        bool bValid = true;
        for (const FAny& Param : Params) bValid &= Param.IsValid();
        return bValid;
    }

    template<typename... Types>
    static bool EventParamsAreCorrectTypes(const TArray<FAny>& Params)
    {
        if (Params.Num() != sizeof...(Types)) return false;
        bool bCorrect = true;
        int32 Index = 0;
        // ReSharper disable once CppAssignedValueIsNeverUsed
        ((bCorrect &= Params[Index++].Get<Types>() != nullptr), ...);
        return bCorrect;
    }
} // namespace Internal

#ifndef PARAMS_ARE_VALID
#define PARAMS_ARE_VALID Internal::EventParamsAreValid(Params)
#endif

#ifndef PARAMS_ARE_CORRECT_TYPES
#define PARAMS_ARE_CORRECT_TYPES(...) Internal::EventParamsAreCorrectTypes<__VA_ARGS__>(Params)
#endif

UINTERFACE(MinimalAPI, BlueprintType)
class UEventListener : public UInterface
{
    GENERATED_BODY()
};

class GADE7322_TD_API IEventListener
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
    void OnEventReceived(const FName& EventName, const TArray<FAny>& Params);
};
