#include "TowerDefencePawns/ProximityPerception/AISenseConfig_Proximity.h"

UAISenseConfig_Proximity::UAISenseConfig_Proximity(const FObjectInitializer& ObjectInitializer) :
    Super(ObjectInitializer)
{
    DebugColor = FColor::Cyan;
}

TSubclassOf<UAISense> UAISenseConfig_Proximity::GetSenseImplementation() const { return *Implementation; }
