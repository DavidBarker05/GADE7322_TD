#pragma once

#include "CoreMinimal.h"

#include "Any/Any.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "EventBus.generated.h"

class UEventBus;

namespace Internal
{
    static UEventBus* GetEventBusFromContext(const UObject* ContextObject)
    {
        if (!IsValid(ContextObject)) return nullptr;
        const UWorld* World = nullptr;
        if (const AActor* Actor = Cast<AActor>(ContextObject)) World = Actor->GetWorld();
        else if (const USceneComponent* SceneComponent = Cast<USceneComponent>(ContextObject))
            World = SceneComponent->GetWorld();
        else if (const UActorComponent* Component = Cast<UActorComponent>(ContextObject))
        {
            if (const AActor* Owner = Component->GetOwner()) World = Owner->GetWorld();
        }
        if (!World) return nullptr;
        if (const UGameInstance* GameInstance = World->GetGameInstance())
            return GameInstance->GetSubsystem<UEventBus>();
        return nullptr;
    }
} // namespace Internal

#ifndef BROADCAST_EVENT
#define BROADCAST_EVENT(EventName, ...) \
    do \
    { \
        if (UEventBus* EventBus = Internal::GetEventBusFromContext(this)) \
            EventBus->Broadcast(FName(EventName), {__VA_ARGS__}); \
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

public:
    UFUNCTION(BlueprintCallable)
    void AddListener(const FName& EventName, TScriptInterface<IEventListener> Listener);

    UFUNCTION(BlueprintCallable)
    void RemoveListener(const FName& EventName, TScriptInterface<IEventListener> Listener);

    UFUNCTION(BlueprintCallable)
    void Broadcast(const FName& EventName, const TArray<FAny>& Params);

private:
    TMap<FName, TArray<TScriptInterface<IEventListener>>> EventListeners;
};
