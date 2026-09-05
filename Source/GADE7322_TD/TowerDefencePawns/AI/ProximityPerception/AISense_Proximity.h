// ReSharper disable CppNonExplicitConvertingConstructor
#pragma once

#include "CoreMinimal.h"

#include "Perception/AISense.h"

#include "AISense_Proximity.generated.h"

class UAISenseConfig_Proximity;

UCLASS()
class GADE7322_TD_API UAISense_Proximity : public UAISense
{
    GENERATED_BODY()

public:
    UAISense_Proximity(const FObjectInitializer& ObjectInitializer);

    virtual void RegisterSource(AActor& SourceActor) override;
    virtual void UnregisterSource(AActor& SourceActor) override;

    // Fires synchronously the moment PerceptionComponent->ForgetAll() is called (e.g. on
    // SetControllerActive(false)/(true)), unlike Update()'s own 0.2s poll - a pooled pawn's death and
    // respawn can both happen well within that 0.2s window, so Update() alone can miss the listener ever
    // being absent and never purge its stale PairsInRange entries. This closes that race
    virtual void OnListenerForgetsAll(const FPerceptionListener& Listener) override;

protected:
    virtual float Update() override;

private:
    TArray<TWeakObjectPtr<AActor>> RegisteredSources;

    TSet<TPair<FPerceptionListenerID, TWeakObjectPtr<AActor>>> PairsInRange;
};
