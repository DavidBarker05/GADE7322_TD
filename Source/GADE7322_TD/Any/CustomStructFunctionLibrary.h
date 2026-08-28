// ReSharper disable CppUEBlueprintCallableFunctionUnused
#pragma once

#include "CoreMinimal.h"

#include "BlueprintFunctionLibraryEnums.h"
#include "CustomStructs.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "CustomStructFunctionLibrary.generated.h"

UCLASS()
class GADE7322_TD_API UCustomStructFunctionLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "CustomStructs|BooleanStruct|Creation",
              meta = (DisplayName = "Make Boolean Struct", ToolTip = "Makes a Boolean Struct based on the Boolean",
                      NativeMakeFunc))
    static FBoolStruct MakeBoolStruct(bool InValue);

    UFUNCTION(BlueprintPure, Category = "CustomStructs|BooleanStruct|Value",
              meta = (DisplayName = "Break Boolean Struct", ReturnDisplayName = "Boolean",
                      ToolTip = "Breaks a Boolean Struct", NativeBreakFunc))
    static bool BreakBoolStruct(const FBoolStruct& BooleanStruct);

    UFUNCTION(BlueprintPure, Category = "CustomStructs|BooleanStruct|Boolean",
              meta = (BlueprintAutocast, DisplayName = "Boolean Struct To Boolean", CompactNodeTitle = "->",
                      ToolTip = "Converts a Boolean Struct to a Boolean"))
    static bool FBoolStruct_To_Bool(const FBoolStruct& A);

    UFUNCTION(BlueprintPure, Category = "CustomStructs|BoolStruct|Boolean",
              meta = (BlueprintAutocast, DisplayName = "Boolean To Boolean Struct", CompactNodeTitle = "->",
                      ToolTip = "Converts a Boolean to a Boolean Struct"))
    static FBoolStruct Bool_ToFBoolStruct(bool A);

    UFUNCTION(BlueprintPure, Category = "CustomStructs|BooleanStruct|Value",
              meta = (DisplayName = "Get", ToolTip = "Gets the Boolean stored in the Boolean Struct"))
    static bool FBoolStruct_Get(const FBoolStruct& InStruct);

    UFUNCTION(BlueprintCallable, Category = "CustomStructs|BooleanStruct|Value",
              meta = (DisplayName = "Set", ToolTip = "Sets the Boolean stored in the Boolean Struct"))
    static FBoolStruct& FBoolStruct_Set(UPARAM(ref) FBoolStruct& InStruct, bool NewValue);

public:
    UFUNCTION(BlueprintCallable, Category = "CustomStructs|ByteStruct|Creation",
              meta = (DisplayName = "Make Byte Struct", ToolTip = "Makes an Byte Struct based on the Byte",
                      NativeMakeFunc))
    static FUint8Struct MakeUint8Struct(uint8 InValue);

    UFUNCTION(BlueprintPure, Category = "CustomStructs|ByteStruct|Value",
              meta = (DisplayName = "Break Byte Struct", ReturnDisplayName = "Byte", ToolTip = "Breaks an Byte Struct",
                      NativeBreakFunc))
    static uint8 BreakUint8Struct(const FUint8Struct& ByteStruct);

    UFUNCTION(BlueprintPure, Category = "CustomStructs|ByteStruct|Byte",
              meta = (BlueprintAutocast, DisplayName = "Byte Struct To Byte", CompactNodeTitle = "->",
                      ToolTip = "Converts an Byte Struct to an Byte"))
    static uint8 FUint8Struct_To_Uint8(const FUint8Struct& A);

    UFUNCTION(BlueprintPure, Category = "CustomStructs|ByteStruct|Byte",
              meta = (BlueprintAutocast, DisplayName = "Byte To Byte Struct", CompactNodeTitle = "->",
                      ToolTip = "Converts an Byte to an Integer Byte"))
    static FUint8Struct Uint8_To_FUint8Struct(uint8 A);

    UFUNCTION(BlueprintPure, Category = "CustomStructs|ByteStruct|Value",
              meta = (DisplayName = "Get", ToolTip = "Gets the Byte stored in the Byte Struct"))
    static uint8 FUint8Struct_Get(const FUint8Struct& InStruct);

    UFUNCTION(BlueprintCallable, Category = "CustomStructs|ByteStruct|Value",
              meta = (DisplayName = "Set", ToolTip = "Sets the Byte stored in the Byte Struct"))
    static FUint8Struct& FUint8Struct_Set(UPARAM(ref) FUint8Struct& InStruct, uint8 NewValue);

public:
    UFUNCTION(BlueprintCallable, Category = "CustomStructs|IntegerStruct|Creation",
              meta = (DisplayName = "Make Integer Struct", ToolTip = "Makes an Integer Struct based on the Integer",
                      NativeMakeFunc))
    static FInt32Struct MakeInt32Struct(int32 InValue);

    UFUNCTION(BlueprintPure, Category = "CustomStructs|IntegerStruct|Value",
              meta = (DisplayName = "Break Integer Struct", ReturnDisplayName = "Integer",
                      ToolTip = "Breaks an Integer Struct", NativeBreakFunc))
    static int32 BreakInt32Struct(const FInt32Struct& IntegerStruct);

    UFUNCTION(BlueprintPure, Category = "CustomStructs|IntegerStruct|Integer",
              meta = (BlueprintAutocast, DisplayName = "Integer Struct To Integer", CompactNodeTitle = "->",
                      ToolTip = "Converts an Integer Struct to an Integer"))
    static int32 FInt32Struct_To_Int32(const FInt32Struct& A);

    UFUNCTION(BlueprintPure, Category = "CustomStructs|IntegerStruct|Integer",
              meta = (BlueprintAutocast, DisplayName = "Integer To Integer Struct", CompactNodeTitle = "->",
                      ToolTip = "Converts an Integer to an Integer Struct"))
    static FInt32Struct Int32_To_FInt32Struct(int32 A);

    UFUNCTION(BlueprintPure, Category = "CustomStructs|IntegerStruct|Value",
              meta = (DisplayName = "Get", ToolTip = "Gets the Integer stored in the Integer Struct"))
    static int32 FInt32Struct_Get(const FInt32Struct& InStruct);

    UFUNCTION(BlueprintCallable, Category = "CustomStructs|IntegerStruct|Value",
              meta = (DisplayName = "Set", ToolTip = "Sets the Integer stored in the Integer Struct"))
    static FInt32Struct& FInt32Struct_Set(UPARAM(ref) FInt32Struct& InStruct, int32 NewValue);

public:
    UFUNCTION(BlueprintCallable, Category = "CustomStructs|Integer64Struct|Creation",
              meta = (DisplayName = "Make Integer64 Struct",
                      ToolTip = "Makes an Integer64 Struct based on the Integer64", NativeMakeFunc))
    static FInt64Struct MakeInt64Struct(int64 InValue);

    UFUNCTION(BlueprintPure, Category = "CustomStructs|Integer64Struct|Value",
              meta = (DisplayName = "Break Integer64 Struct", ReturnDisplayName = "Integer64",
                      ToolTip = "Breaks an Integer64 Struct", NativeBreakFunc))
    static int64 BreakInt64Struct(const FInt64Struct& Integer64Struct);

    UFUNCTION(BlueprintPure, Category = "CustomStructs|Integer64Struct|Integer64",
              meta = (BlueprintAutocast, DisplayName = "Integer64 Struct To Integer64", CompactNodeTitle = "->",
                      ToolTip = "Converts an Integer64 Struct to an Integer64"))
    static int64 FInt64Struct_To_Int64(const FInt64Struct& A);

    UFUNCTION(BlueprintPure, Category = "CustomStructs|Integer64Struct|Integer64",
              meta = (BlueprintAutocast, DisplayName = "Integer64 To Integer64 Struct", CompactNodeTitle = "->",
                      ToolTip = "Converts an Integer64 to an Integer64 Struct"))
    static FInt64Struct Int64_To_FInt64Struct(int64 A);

    UFUNCTION(BlueprintPure, Category = "CustomStructs|Intege64rStruct|Value",
              meta = (DisplayName = "Get", ToolTip = "Gets the Integer64 stored in the Integer64 Struct"))
    static int64 FInt64Struct_Get(const FInt64Struct& InStruct);

    UFUNCTION(BlueprintCallable, Category = "CustomStructs|IntegerStruct|Value",
              meta = (DisplayName = "Set", ToolTip = "Sets the Integer stored in the Integer Struct"))
    static FInt64Struct& FInt64Struct_Set(UPARAM(ref) FInt64Struct& InStruct, int64 NewValue);

public:
    UFUNCTION(BlueprintCallable, Category = "CustomStructs|FloatStruct|Creation",
              meta = (DisplayName = "Make Float Struct", ToolTip = "Makes a Float Struct based on the Float",
                      NativeMakeFunc))
    static FFloatStruct MakeFloatStruct(float InValue);

    UFUNCTION(BlueprintPure, Category = "CustomStructs|FloatStruct|Value",
              meta = (DisplayName = "Break Float Struct", ReturnDisplayName = "Float",
                      ToolTip = "Breaks a Float Struct", NativeBreakFunc))
    static float BreakFloatStruct(const FFloatStruct& FloatStruct);

    UFUNCTION(BlueprintPure, Category = "CustomStructs|FloatStruct|Float",
              meta = (BlueprintAutocast, DisplayName = "Float Struct To Float", CompactNodeTitle = "->",
                      ToolTip = "Converts a Float Struct to a Float"))
    static float FFloatStruct_To_Float(const FFloatStruct& A);

    UFUNCTION(BlueprintPure, Category = "CustomStructs|FloatStruct|Float",
              meta = (BlueprintAutocast, DisplayName = "Float To Float Struct", CompactNodeTitle = "->",
                      ToolTip = "Converts a Float to a Float Struct"))
    static FFloatStruct Float_To_FFloatStruct(float A);

    UFUNCTION(BlueprintPure, Category = "CustomStructs|FloatStruct|Value",
              meta = (DisplayName = "Get", ToolTip = "Gets the Float stored in the Float Struct"))
    static float FFloatStruct_Get(const FFloatStruct& InStruct);

    UFUNCTION(BlueprintCallable, Category = "CustomStructs|FloatStruct|Value",
              meta = (DisplayName = "Set", ToolTip = "Sets the Float stored in the Float Struct"))
    static FFloatStruct& FFloatStruct_Set(UPARAM(ref) FFloatStruct& InStruct, float NewValue);

public:
    UFUNCTION(BlueprintCallable, Category = "CustomStructs|StringStruct|Creation",
              meta = (DisplayName = "Make String Struct", ToolTip = "Makes a String Struct based on the String",
                      NativeMakeFunc))
    static FFStringStruct MakeFStringStruct(const FString& InValue);

    UFUNCTION(BlueprintPure, Category = "CustomStructs|StringStruct|Value",
              meta = (DisplayName = "Break String Struct", ReturnDisplayName = "String",
                      ToolTip = "Breaks a String Struct", NativeBreakFunc))
    static FString BreakFStringStruct(const FFStringStruct& StringStruct);

    UFUNCTION(BlueprintPure, Category = "CustomStructs|StringStruct|String",
              meta = (BlueprintAutocast, DisplayName = "String Struct To String", CompactNodeTitle = "->",
                      ToolTip = "Converts a String Struct to a String"))
    static FString FFStringStruct_To_FString(const FFStringStruct& A);

    UFUNCTION(BlueprintPure, Category = "CustomStructs|StringStruct|String",
              meta = (BlueprintAutocast, DisplayName = "String To String Struct", CompactNodeTitle = "->",
                      ToolTip = "Converts a String to a String Struct"))
    static FFStringStruct FString_To_FFStringStruct(const FString& A);

    UFUNCTION(BlueprintPure, Category = "CustomStructs|StringStruct|Value",
              meta = (DisplayName = "Get", ToolTip = "Gets the String stored in the String Struct"))
    static FString FFStringStruct_Get(const FFStringStruct& InStruct);

    UFUNCTION(BlueprintCallable, Category = "CustomStructs|StringStruct|Value",
              meta = (DisplayName = "Set", ToolTip = "Sets the String stored in the String Struct"))
    static FFStringStruct& FFStringStruct_Set(UPARAM(ref) FFStringStruct& InStruct, const FString& NewValue);

public:
    UFUNCTION(BlueprintCallable, Category = "CustomStructs|NameStruct|Creation",
              meta = (DisplayName = "Make Name Struct", ToolTip = "Makes a Name Struct based on the Name",
                      NativeMakeFunc))
    static FFNameStruct MakeFNameStruct(const FName& InValue);

    UFUNCTION(BlueprintPure, Category = "CustomStructs|NameStruct|Value",
              meta = (DisplayName = "Break Name Struct", ReturnDisplayName = "Name", ToolTip = "Breaks a Name Struct",
                      NativeBreakFunc))
    static FName BreakFNameStruct(const FFNameStruct& NameStruct);

    UFUNCTION(BlueprintPure, Category = "CustomStructs|NameStruct|Name",
              meta = (BlueprintAutocast, DisplayName = "Name Struct To Name", CompactNodeTitle = "->",
                      ToolTip = "Converts a Name Struct to a Name"))
    static FName FFNameStruct_To_FName(const FFNameStruct& A);

    UFUNCTION(BlueprintPure, Category = "CustomStructs|NameStruct|Name",
              meta = (BlueprintAutocast, DisplayName = "Name To Name Struct", CompactNodeTitle = "->",
                      ToolTip = "Converts a Name to a Name Struct"))
    static FFNameStruct FName_To_FFNameStruct(const FName& A);

    UFUNCTION(BlueprintPure, Category = "CustomStructs|NameStruct|Value",
              meta = (DisplayName = "Get", ToolTip = "Gets the Name stored in the Name Struct"))
    static FName FFNameStruct_Get(const FFNameStruct& InStruct);

    UFUNCTION(BlueprintCallable, Category = "CustomStructs|NameStruct|Value",
              meta = (DisplayName = "Set", ToolTip = "Sets the Name stored in the Name Struct"))
    static FFNameStruct& FFNameStruct_Set(UPARAM(ref) FFNameStruct& InStruct, const FName& NewValue);

public:
    UFUNCTION(BlueprintCallable, Category = "CustomStructs|TextStruct|Creation",
              meta = (DisplayName = "Make Text Struct", ToolTip = "Makes a Text Struct based on the Text",
                      NativeMakeFunc))
    static FFTextStruct MakeFTextStruct(const FText& InValue);

    UFUNCTION(BlueprintPure, Category = "CustomStructs|TextStruct|Value",
              meta = (DisplayName = "Break Text Struct", ReturnDisplayName = "Text", ToolTip = "Breaks a Text Struct",
                      NativeBreakFunc))
    static FText BreakFTextStruct(const FFTextStruct& TextStruct);

    UFUNCTION(BlueprintPure, Category = "CustomStructs|TextStruct|Text",
              meta = (BlueprintAutocast, DisplayName = "Text Struct To Text", CompactNodeTitle = "->",
                      ToolTip = "Converts a Text Struct to a Text"))
    static FText FFTextStruct_To_FText(const FFTextStruct& A);

    UFUNCTION(BlueprintPure, Category = "CustomStructs|TextStruct|Text",
              meta = (BlueprintAutocast, DisplayName = "Text To Text Struct", CompactNodeTitle = "->",
                      ToolTip = "Converts a Text to a Text Struct"))
    static FFTextStruct FText_ToFFTextStruct(const FText& A);

    UFUNCTION(BlueprintPure, Category = "CustomStructs|TextStruct|Value",
              meta = (DisplayName = "Get", ToolTip = "Gets the Text stored in the Text Struct"))
    static FText FFTextStruct_Get(const FFTextStruct& InStruct);

    UFUNCTION(BlueprintCallable, Category = "CustomStructs|TextStruct|Value",
              meta = (DisplayName = "Set", ToolTip = "Sets the Text stored in the Text Struct"))
    static FFTextStruct& FFTextStruct_Set(UPARAM(ref) FFTextStruct& InStruct, const FText& NewValue);

public:
    UFUNCTION(BlueprintCallable, Category = "CustomStructs|ObjectStruct|Creation",
              meta = (DisplayName = "Make Object Struct", ToolTip = "Makes an Object Struct based on the Object",
                      NativeMakeFunc))
    static FUObjectStruct MakeUObjectStruct(UObject* InValue);

    UFUNCTION(BlueprintPure, Category = "CustomStructs|ObjectStruct|Value",
              meta = (DisplayName = "Break Object Struct", ReturnDisplayName = "Object",
                      ToolTip = "Breaks an Object Struct", NativeBreakFunc))
    static UObject* BreakUObjectStruct(const FUObjectStruct& ObjectStruct);

    UFUNCTION(BlueprintPure, Category = "CustomStructs|ObjectStruct|Object",
              meta = (BlueprintAutocast, DisplayName = "Object Struct To Object", CompactNodeTitle = "->",
                      ToolTip = "Converts an Object Struct to an Object"))
    static UObject* FUObjectStruct_To_UObject(const FUObjectStruct& A);

    UFUNCTION(BlueprintPure, Category = "CustomStructs|ObjectStruct|Object",
              meta = (BlueprintAutocast, DisplayName = "Object To Object Struct", CompactNodeTitle = "->",
                      ToolTip = "Converts an Object to an Object Struct"))
    static FUObjectStruct UObject_To_FUObjectStruct(UObject* A);

    UFUNCTION(BlueprintPure, Category = "CustomStructs|ObjectStruct|Value",
              meta = (DisplayName = "Get", ToolTip = "Gets the Object stored in the Object Struct"))
    static UObject* FUObjectStruct_Get(const FUObjectStruct& InStruct);

    UFUNCTION(BlueprintCallable, Category = "CustomStructs|ObjectStruct|Value",
              meta = (DisplayName = "Get As Type", DeterminesOutputType = "Class",
                      ToolTip = "Gets the Object stored in the Object Struct as the class type",
                      ExpandEnumAsExecs = "OutputPins"))
    static UObject* FUObjectStruct_GetAsType(const FUObjectStruct& ObjectStruct, TSubclassOf<UObject> Class,
                                             EIsAOutputPins& OutputPins);

    UFUNCTION(BlueprintCallable, Category = "CustomStructs|ObjectStruct|Value",
              meta = (DisplayName = "Set", ToolTip = "Sets the Object stored in the Object Struct"))
    static FUObjectStruct& FUObjectStruct_Set(UPARAM(ref) FUObjectStruct& InStruct, UObject* NewValue);
};
