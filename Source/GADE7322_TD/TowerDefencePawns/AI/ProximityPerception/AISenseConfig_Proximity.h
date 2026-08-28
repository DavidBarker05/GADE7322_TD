// ReSharper disable CppNonExplicitConvertingConstructor
#pragma once

#include "CoreMinimal.h"

#include "Perception/AISenseConfig.h"
#include "TowerDefencePawns/AI/ProximityPerception/AISense_Proximity.h"

#include "AISenseConfig_Proximity.generated.h"

UCLASS(meta = (DisplayName = "AI Proximity config"))
class GADE7322_TD_API UAISenseConfig_Proximity : public UAISenseConfig
{
    GENERATED_BODY()

public:
    UAISenseConfig_Proximity(const FObjectInitializer& ObjectInitializer);

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sense", NoClear, config)
    TSubclassOf<UAISense_Proximity> Implementation;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sense", config,
              meta = (UIMin = 0.0, ClampMin = 0.0, Units = "Centimeters"))
    float DetectionRadius;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sense", config)
    FAISenseAffiliationFilter DetectionByAffiliation;

    virtual TSubclassOf<UAISense> GetSenseImplementation() const override;
};
