#pragma once

#include "CoreMinimal.h"

#include "BlueprintFunctionLibraryEnums.Generated.h"

UENUM()
enum class EIsValidOutputPins : uint8
{
    Valid,
    NotValid
};

UENUM()
enum class EIsAOutputPins : uint8
{
    IsType,
    IsNotType
};
