#pragma once

#include "CoreMinimal.h"

#include "UObject/ObjectPtr.h"

#include "CustomStructs.generated.h"

#ifndef STRUCT
#define STRUCT(Prefix) F##Prefix##Struct
#endif

#ifndef GENERATE_STRUCT_BOILERPLATE
#define GENERATE_STRUCT_BOILERPLATE(Type, Prefix) \
    STRUCT(Prefix)() = default; \
    STRUCT(Prefix)(const STRUCT(Prefix)&) = default; \
    STRUCT(Prefix)(STRUCT(Prefix)&&) noexcept = default; \
    \
    STRUCT(Prefix)(Type&& InValue) : Value(InValue) \
    { \
    } \
    \
    inline STRUCT(Prefix)& operator=(const STRUCT(Prefix)&) = default; \
    inline STRUCT(Prefix)& operator=(STRUCT(Prefix)&&) noexcept = default; \
    \
    inline STRUCT(Prefix)& operator=(Type&& OtherValue) \
    { \
        Value = OtherValue; \
        return *this; \
    } \
    \
    inline operator Type() const { return Value; } \
    \
    inline Type Get() const { return Value; } \
    \
    inline STRUCT(Prefix)& Set(const Type& NewValue) \
    { \
        Value = NewValue; \
        return *this; \
    } \
    inline STRUCT(Prefix)& Set(Type&& NewValue) \
    { \
        Value = MoveTemp(NewValue); \
        return *this; \
    }
#endif

#ifndef TYPE_OPERATOR_BOILERPLATE
#define TYPE_OPERATOR_BOILERPLATE(Type, Body, ...) inline operator Type() __VA_ARGS__ { Body; }
#endif

#ifndef EXTRA_BOILERPLATE
#define EXTRA_BOILERPLATE

#define EXTRA_CONSTRUCTOR_BOILERPLATE(Type, Prefix) STRUCT(Prefix)(Type InValue) : Value(InValue) { }

#define EXTRA_ASSIGN_BOILERPLATE(Type, Prefix) \
    inline STRUCT(Prefix)& operator=(Type OtherValue) \
    { \
        Value = OtherValue; \
        return *this; \
    }
#endif

#ifndef CHAR_TYPES_BOILERPLATE
#define CHAR_TYPES_BOILERPLATE(Macro, Prefix) \
    Macro(const char*, Prefix) \
    Macro(const WIDECHAR*, Prefix) \
    Macro(char*, Prefix) \
    Macro(WIDECHAR*, Prefix)
#endif

USTRUCT(BlueprintType,
    meta = (DisplayName = "Boolean Struct", HasNativeMake =
        "/Script/GADE7322_TD.CustomStructFunctionLibrary.MakeBoolStruct", HasNativeBreak =
        "/Script/GADE7322_TD.CustomStructFunctionLibrary.BreakBoolStruct"))
struct FBoolStruct
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool Value{};

    GENERATE_STRUCT_BOILERPLATE(bool, Bool)

    EXTRA_CONSTRUCTOR_BOILERPLATE(const bool&, Bool)
};

USTRUCT(BlueprintType,
    meta = (DisplayName = "Byte Struct", HasNativeMake =
        "/Script/GADE7322_TD.CustomStructFunctionLibrary.MakeUint8Struct", HasNativeBreak =
        "/Script/GADE7322_TD.CustomStructFunctionLibrary.BreakUint8Struct"))
struct FUint8Struct
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    uint8 Value{};

    GENERATE_STRUCT_BOILERPLATE(uint8, Uint8)

    EXTRA_CONSTRUCTOR_BOILERPLATE(const uint8&, Uint8)
};

USTRUCT(BlueprintType,
    meta = (DisplayName = "Integer Struct", HasNativeMake =
        "/Script/GADE7322_TD.CustomStructFunctionLibrary.MakeInt32Struct", HasNativeBreak =
        "/Script/GADE7322_TD.CustomStructFunctionLibrary.BreakInt32Struct"))
struct FInt32Struct
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Value{};

    GENERATE_STRUCT_BOILERPLATE(int32, Int32)

    EXTRA_CONSTRUCTOR_BOILERPLATE(const int32&, Int32)
};

USTRUCT(BlueprintType,
    meta = (DisplayName = "Integer64 Struct", HasNativeMake =
        "/Script/GADE7322_TD.CustomStructFunctionLibrary.MakeInt64Struct", HasNativeBreak =
        "/Script/GADE7322_TD.CustomStructFunctionLibrary.BreakInt64Struct"))
struct FInt64Struct
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int64 Value{};

    GENERATE_STRUCT_BOILERPLATE(int64, Int64)

    EXTRA_CONSTRUCTOR_BOILERPLATE(const int64&, Int64)
};

USTRUCT(BlueprintType,
    meta = (DisplayName = "Float Struct", HasNativeMake =
        "/Script/GADE7322_TD.CustomStructFunctionLibrary.MakeFloatStruct", HasNativeBreak =
        "/Script/GADE7322_TD.CustomStructFunctionLibrary.BreakFloatStruct"))
struct FFloatStruct
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Value{};

    GENERATE_STRUCT_BOILERPLATE(float, Float)

    EXTRA_CONSTRUCTOR_BOILERPLATE(const float&, Float)
};

USTRUCT(BlueprintType,
    meta = (DisplayName = "String Struct", HasNativeMake =
        "/Script/GADE7322_TD.CustomStructFunctionLibrary.MakeFStringStruct", HasNativeBreak =
        "/Script/GADE7322_TD.CustomStructFunctionLibrary.BreakFStringStruct"))
struct FFStringStruct
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Value{};

    GENERATE_STRUCT_BOILERPLATE(FString, FString)
    EXTRA_CONSTRUCTOR_BOILERPLATE(const FString&, FString)
    CHAR_TYPES_BOILERPLATE(EXTRA_CONSTRUCTOR_BOILERPLATE, FString)

    CHAR_TYPES_BOILERPLATE(EXTRA_ASSIGN_BOILERPLATE, FString)
};

USTRUCT(BlueprintType,
    meta = (DisplayName = "Name Struct", HasNativeMake =
        "/Script/GADE7322_TD.CustomStructFunctionLibrary.MakeFNameStruct", HasNativeBreak =
        "/Script/GADE7322_TD.CustomStructFunctionLibrary.BreakFNameStruct"))
struct FFNameStruct
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName Value{};

    GENERATE_STRUCT_BOILERPLATE(FName, FName)

    EXTRA_CONSTRUCTOR_BOILERPLATE(const FName&, FName)
    EXTRA_CONSTRUCTOR_BOILERPLATE(const FString&, FName)
    CHAR_TYPES_BOILERPLATE(EXTRA_CONSTRUCTOR_BOILERPLATE, FName)

    CHAR_TYPES_BOILERPLATE(EXTRA_ASSIGN_BOILERPLATE, FName)
};

USTRUCT(BlueprintType,
    meta = (DisplayName = "Text Struct",
        HasNativeMake = "/Script/GADE7322_TD.CustomStructFunctionLibrary.MakeFTextStruct",
        HasNativeBreak = "/Script/GADE7322_TD.CustomStructFunctionLibrary.BreakFTextStruct"))
struct GADE7322_TD_API FFTextStruct
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText Value = FText::GetEmpty();

    GENERATE_STRUCT_BOILERPLATE(FText, FText)

    EXTRA_CONSTRUCTOR_BOILERPLATE(const FText&, FText)
};

USTRUCT(BlueprintType,
    meta = (DisplayName = "Object Struct",
        HasNativeMake = "/Script/GADE7322_TD.CustomStructFunctionLibrary.MakeUObjectStruct",
        HasNativeBreak = "/Script/GADE7322_TD.CustomStructFunctionLibrary.BreakUObjectStruct"))
struct GADE7322_TD_API FUObjectStruct
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UObject> Value = nullptr;

    GENERATE_STRUCT_BOILERPLATE(TObjectPtr<UObject>, UObject)

    EXTRA_CONSTRUCTOR_BOILERPLATE(UObject*, UObject)

    EXTRA_ASSIGN_BOILERPLATE(TObjectPtr<UObject>, UObject)
    EXTRA_ASSIGN_BOILERPLATE(UObject*, UObject)

    TYPE_OPERATOR_BOILERPLATE(UObject*, return Value, const)
};

#undef CHAR_TYPES_BOILERPLATE
#undef EXTRA_BOILERPLATE
#undef TYPE_OPERATOR_BOILERPLATE
#undef GENERATE_STRUCT_BOILERPLATE
#undef STRUCT
