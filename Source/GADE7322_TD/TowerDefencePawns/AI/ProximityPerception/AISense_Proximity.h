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

protected:
    virtual float Update() override;

private:
    TArray<TWeakObjectPtr<AActor>> RegisteredSources;

    TSet<TPair<FPerceptionListenerID, TWeakObjectPtr<AActor>>> PairsInRange;
};
