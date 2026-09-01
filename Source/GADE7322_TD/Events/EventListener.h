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
        if (const UWorld* World = this->GetWorld(); const UGameInstance* GameInstance = World->GetGameInstance()) \
        { \
            if (UEventBus* EventBus = GameInstance->GetSubsystem<UEventBus>()) \
            { \
                for (const FString& EventName : EventNames) EventBus->AddListener(FName(EventName), this); \
            } \
        } \
    } while (0)
#endif

#ifndef SUBSCRIBE_TO_EVENTS_RUNTIME
// Subscribe to new events during runtime, write this in the cpp file inside a function
#define SUBSCRIBE_TO_EVENTS_RUNTIME(...) \
    do \
    { \
        if (const UWorld* World = this->GetWorld(); const UGameInstance* GameInstance = World->GetGameInstance()) \
        { \
            if (UEventBus* EventBus = GameInstance->GetSubsystem<UEventBus>()) \
            { \
                FString TempNames[] = {__VA_ARGS__}; \
                for (const FString& Name : TempNames) \
                { \
                    if (EventNames.Contains(Name)) continue; \
                    (void)EventNames.Add(Name); \
                    EventBus->AddListener(FName(Name), this); \
                } \
            } \
        } \
    } while (0)
#endif

#ifndef UNSUBSCRIBE_FROM_EVENTS
// Write this in the cpp file in the EndPlay function
#define UNSUBSCRIBE_FROM_EVENTS() \
    do \
    { \
        if (const UWorld* World = this->GetWorld(); const UGameInstance* GameInstance = World->GetGameInstance()) \
        { \
            if (UEventBus* EventBus = GameInstance->GetSubsystem<UEventBus>()) \
            { \
                for (const FString& EventName : EventNames) EventBus->RemoveListener(FName(EventName), this); \
            } \
        } \
    } while (0)
#endif

#ifndef UNSUBSCRIBE_FROM_EVENTS_RUNTIME
// Unsubscribe from events during runtime, write this in the cpp file inside a function
#define UNSUBSCRIBE_FROM_EVENTS_RUNTIME(...) \
    do \
    { \
        if (const UWorld* World = this->GetWorld(); const UGameInstance* GameInstance = World->GetGameInstance()) \
        { \
            if (UEventBus* EventBus = GameInstance->GetSubsystem<UEventBus>()) \
            { \
                FString TempNames[] = {__VA_ARGS__}; \
                for (const FString& Name : TempNames) \
                { \
                    if (!EventNames.Contains(Name)) continue; \
                    (void)EventNames.Remove(Name); \
                    EventBus->RemoveListener(FName(Name), this); \
                } \
            } \
        } \
    } while (0)
#endif

#ifndef EVENT_MATCHES
#define EVENT_MATCHES(Name, NumArgs) EventName.IsEqual(Name) && Params.Num() == NumArgs
#endif

#ifndef PARAMS_ARE_VALID
#define PARAMS_ARE_VALID \
    [&Params]() -> bool \
    { \
        bool bValid = true; \
        for (const FAny& Param : Params) bValid &= Param.IsValid(); \
        return bValid; \
    }()
#endif

#ifndef PARAMS_ARE_CORRECT_TYPES
#define PARAMS_ARE_CORRECT_TYPES(...) \
    [&Params]<typename... Types>() -> bool \
    { \
        if (Params.Num() != sizeof...(Types)) return false; \
        bool bCorrect = true; \
        int32 Index = 0; \
        ((bCorrect &= Params[Index++].Get<Types>() != nullptr), ...); \
        return bCorrect; \
    }.template operator()<__VA_ARGS__>()
#endif

UINTERFACE(MinimalAPI, BlueprintType) class UEventListener : public UInterface
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
