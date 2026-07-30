// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Kismet/BlueprintFunctionLibrary.h"
#include "Any/Any.h"
#include "StructUtils/InstancedStruct.h"
#include "BlueprintFunctionLibraryEnums.h"

#include "AnyFunctionLibrary.generated.h"

UCLASS()
class GADE7322_TD_API UAnyFunctionLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    // Creates Any based on an input struct (wildcard)
    UFUNCTION(BlueprintCallable, CustomThunk, Category = "CustomStructs|Any|Creation",
        meta = (DisplayName = "Make Any", CustomStructureParam = "Value",
            NativeMakeFunc))
    static FAny MakeAny(const int32& Value);

    UFUNCTION(BlueprintPure, Category = "CustomStructs|Any|Value",
        meta = (DisplayName = "Is Valid", ReturnDisplayName = "Is Valid", ToolTip =
            "Check if the Any contains valid data"))
    static bool Any_IsValid(const FAny& Any);

    UFUNCTION(BlueprintCallable, Category = "CustomStructs|Any|Value",
        meta = (DisplayName = "Is Valid (Branch)", ToolTip = "Check if the Any contains valid data",
            ExpandEnumAsExecs = "OutputPins"))
    static void Any_IsValidBranch(const FAny& Any, EIsValidOutputPins& OutputPins);

    UFUNCTION(BlueprintCallable, CustomThunk, Category = "CustomStructs|Any|Value",
        meta = (DisplayName = "Get", ExpandEnumAsExecs = "OutputPins", CustomStructureParam = "Value", ToolTip
            = "Gets the stored value in the Any\nNote the output pin needs to be used in order for the wildcard type to be set\nMake sure to only use the output if the Is Valid pin executes"
            , BlueprintInternalUseOnly = "true"))
    static void AnyGet(UPARAM(ref) const FAny& InAny, EIsAOutputPins& OutputPins,
                       int32& Value);

    UFUNCTION(BlueprintCallable, CustomThunk, Category = "CustomStructs|Any|Value",
        meta = (CustomStructureParam = "Value", BlueprintInternalUseOnly = "true"))
    static FAny SetAny(const int32& Value);

    // Creates Any based on an input (wildcard)
    UFUNCTION(BlueprintPure, CustomThunk, Category = "CustomStructs|Any|Creation",
        meta = (BlueprintAutoCast, DisplayName = "To Any", CompactNodeTitle = "->", ToolTip =
            "Converts a Struct to Any", CustomStructureParam = "Value"))
    static FAny ToAny(const int32& Value);

private:
    DECLARE_FUNCTION(execMakeAny);

    DECLARE_FUNCTION(execAnyGet);

    DECLARE_FUNCTION(execSetAny);

    DECLARE_FUNCTION(execToAny);
};
