#pragma once

#include "CoreMinimal.h"

#include "Any/Any.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "EventBus.generated.h"

#ifndef BROADCAST_EVENT
#define BROADCAST_EVENT(EventName, ...) \
    do \
    { \
        if (const UWorld* World = this->GetWorld(); const UGameInstance* GameInstance = World->GetGameInstance()) \
        { \
            if (UEventBus* EventBus = GameInstance->GetSubsystem<UEventBus>()) \
                EventBus->Broadcast(FName(EventName), {__VA_ARGS__}); \
        } \
    } while (0)
#endif

class IEventListener;

UCLASS()
class GADE7322_TD_API UEventBus : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    UFUNCTION(BlueprintCallable)
    void AddListener(const FName& EventName, TScriptInterface<IEventListener> Listener);

    UFUNCTION(BlueprintCallable)
    void RemoveListener(const FName& EventName, TScriptInterface<IEventListener> Listener);

    UFUNCTION(BlueprintCallable)
    void Broadcast(const FName& EventName, const TArray<FAny>& Params);

private:
    TMap<FName, TArray<TScriptInterface<IEventListener>>> EventListeners;
};
