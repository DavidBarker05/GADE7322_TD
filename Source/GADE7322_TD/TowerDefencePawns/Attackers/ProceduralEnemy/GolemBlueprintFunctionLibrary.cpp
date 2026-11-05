#include "TowerDefencePawns/Attackers/ProceduralEnemy/GolemBlueprintFunctionLibrary.h"

EGolemPartStrength UGolemBlueprintFunctionLibrary::GetRandomPartStrength()
{
    switch (FMath::RandRange(0, 2))
    {
        case 0:
            return EGolemPartStrength::Weak;
        case 1:
            return EGolemPartStrength::Average;
        case 2:
            return EGolemPartStrength::Strong;
        default:
            return EGolemPartStrength::Unknown;
    }
}
