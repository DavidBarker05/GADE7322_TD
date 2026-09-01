#pragma once

#include "CoreMinimal.h"

#include "UObject/ObjectPtr.h"

#include "CustomStructs.generated.h"

// These structs are USTRUCT wrappers to store types
// inside of them so that they can be used for FInstancedStruct
// inside FAny

// Converts Prefix to FPrefixStruct
// e.g. Int32 -> FInt32Struct
#ifndef STRUCT
#define STRUCT(Prefix) F##Prefix##Struct
#endif

// Okay so I am noticing some stuff that I need to
// check out what happens if I add some copy versions
// on top of the move ones when I am not on my phone
// tomorrow. I should also see what happens when I mark the moves
// as noexcept

// Expands out to boilerplate at compile time that every struct uses.
// This makes the code shorter and I don't have to write the same stuff
// over and over again
#ifndef GENERATE_STRUCT_BOILERPLATE
#define GENERATE_STRUCT_BOILERPLATE(Type, Prefix) \
    /* Default Constructor */ \
    STRUCT(Prefix)() = default; \
\
    /* Copy Constructor */ \
    STRUCT(Prefix)(const STRUCT(Prefix) &) = default; \
\
    /* Move Constructor */ \
    STRUCT(Prefix)(STRUCT(Prefix) &&) noexcept = default; \
\
    /* Constructor that takes in underlying type as an */ \
    /* r-value reference. Move only, because copy caused */ \
    /* issues with FUObjectStruct so other structs just */ \
    /* implement a copy version separately */ \
    STRUCT(Prefix)(Type && InValue) : Value(MoveTemp(InValue)) { } \
\
    /* Copy assignment operator */ \
    inline STRUCT(Prefix) & operator=(const STRUCT(Prefix) &) = default; \
\
    /* Move assignment operator */ \
    inline STRUCT(Prefix) & operator=(STRUCT(Prefix) &&) noexcept = default; \
\
    /* Assignment operator that takes in the underlying type */ \
    /* Why is there no optional copy? Idk, need to test tomorrow */ \
    inline STRUCT(Prefix) & operator=(Type&& OtherValue) \
    { \
        Value = OtherValue; \
        return *this; \
    } \
\
    /* Type operator overload to allow the struct to be */ \
    /* implicitly treates as its underlying type */ \
    inline operator Type() const { return Value; } \
\
    /* Get the underlying data */ \
    inline Type Get() const { return Value; } \
\
    /* Copy Set for the underlying data */ \
    inline STRUCT(Prefix) & Set(const Type& NewValue) \
    { \
        Value = NewValue; \
        return *this; \
    } \
\
    /* Move Set for the underlying data */ \
    inline STRUCT(Prefix) & Set(Type&& NewValue) \
    { \
        Value = MoveTemp(NewValue); \
        return *this; \
    }
#endif

// Yay I can do normal comments again because
// no longer in a macro! :)

#ifndef EXTRA_BOILERPLATE
#define EXTRA_BOILERPLATE

#define EXTRA_CONSTRUCTOR_BOILERPLATE(Type, Prefix) \
    STRUCT(Prefix)(Type InValue) : Value(InValue) { }

#define EXTRA_ASSIGN_BOILERPLATE(Type, Prefix) \
    inline STRUCT(Prefix) & operator=(Type OtherValue) \
    { \
        Value = OtherValue; \
        return *this; \
    }

// Optional additional implicit type overload
// Type = Type to overload
// Body = Body of the function
// __VA_ARGS__ = optional const
#define EXTRA_TYPE_BOILERPLATE(Type, Body, ...) \
    inline operator Type() __VA_ARGS__ { Body; }
#endif

// Create additional boilerplate for different char types
// const char*, char*, const WIDECHAR* and WIDECHAR*
// maybe am supposed to use ANSICHAR, need to look into it
#ifndef CHAR_TYPES_BOILERPLATE
#define CHAR_TYPES_BOILERPLATE(Macro, Prefix) \
    Macro(const char*, Prefix) Macro(const WIDECHAR*, Prefix) Macro(char*, Prefix) Macro(WIDECHAR*, Prefix)
#endif

USTRUCT(BlueprintType, meta = (DisplayName = "Boolean Struct",
                               HasNativeMake = "/Script/GADE7322_TD.CustomStructFunctionLibrary.MakeBoolStruct",
                               HasNativeBreak = "/Script/GADE7322_TD.CustomStructFunctionLibrary.BreakBoolStruct"))
struct FBoolStruct
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool Value {};

    GENERATE_STRUCT_BOILERPLATE(bool, Bool)

    // Copy constructor for underlying type
    EXTRA_CONSTRUCTOR_BOILERPLATE(const bool&, Bool)
};

USTRUCT(BlueprintType, meta = (DisplayName = "Byte Struct",
                               HasNativeMake = "/Script/GADE7322_TD.CustomStructFunctionLibrary.MakeUint8Struct",
                               HasNativeBreak = "/Script/GADE7322_TD.CustomStructFunctionLibrary.BreakUint8Struct"))
struct FUint8Struct
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    uint8 Value {};

    GENERATE_STRUCT_BOILERPLATE(uint8, Uint8)

    // Copy constructor for underlying type
    EXTRA_CONSTRUCTOR_BOILERPLATE(const uint8&, Uint8)
};

USTRUCT(BlueprintType, meta = (DisplayName = "Integer Struct",
                               HasNativeMake = "/Script/GADE7322_TD.CustomStructFunctionLibrary.MakeInt32Struct",
                               HasNativeBreak = "/Script/GADE7322_TD.CustomStructFunctionLibrary.BreakInt32Struct"))
struct FInt32Struct
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Value {};

    GENERATE_STRUCT_BOILERPLATE(int32, Int32)

    // Copy constructor for underlying type
    EXTRA_CONSTRUCTOR_BOILERPLATE(const int32&, Int32)
};

USTRUCT(BlueprintType, meta = (DisplayName = "Integer64 Struct",
                               HasNativeMake = "/Script/GADE7322_TD.CustomStructFunctionLibrary.MakeInt64Struct",
                               HasNativeBreak = "/Script/GADE7322_TD.CustomStructFunctionLibrary.BreakInt64Struct"))
struct FInt64Struct
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int64 Value {};

    GENERATE_STRUCT_BOILERPLATE(int64, Int64)

    // Copy constructor for underlying type
    EXTRA_CONSTRUCTOR_BOILERPLATE(const int64&, Int64)
};

USTRUCT(BlueprintType, meta = (DisplayName = "Float Struct",
                               HasNativeMake = "/Script/GADE7322_TD.CustomStructFunctionLibrary.MakeFloatStruct",
                               HasNativeBreak = "/Script/GADE7322_TD.CustomStructFunctionLibrary.BreakFloatStruct"))
struct FFloatStruct
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Value {};

    GENERATE_STRUCT_BOILERPLATE(float, Float)

    // Copy constructor for underlying type
    EXTRA_CONSTRUCTOR_BOILERPLATE(const float&, Float)
};

USTRUCT(BlueprintType, meta = (DisplayName = "String Struct",
                               HasNativeMake = "/Script/GADE7322_TD.CustomStructFunctionLibrary.MakeFStringStruct",
                               HasNativeBreak = "/Script/GADE7322_TD.CustomStructFunctionLibrary.BreakFStringStruct"))
struct FFStringStruct
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Value {};

    GENERATE_STRUCT_BOILERPLATE(FString, FString)

    // Copy constructor for underlying type
    EXTRA_CONSTRUCTOR_BOILERPLATE(const FString&, FString)

    // Extra constructors for different char types
    CHAR_TYPES_BOILERPLATE(EXTRA_CONSTRUCTOR_BOILERPLATE, FString)

    // Extra assignment operators for different char types
    CHAR_TYPES_BOILERPLATE(EXTRA_ASSIGN_BOILERPLATE, FString)
};

USTRUCT(BlueprintType, meta = (DisplayName = "Name Struct",
                               HasNativeMake = "/Script/GADE7322_TD.CustomStructFunctionLibrary.MakeFNameStruct",
                               HasNativeBreak = "/Script/GADE7322_TD.CustomStructFunctionLibrary.BreakFNameStruct"))
struct FFNameStruct
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName Value {};

    GENERATE_STRUCT_BOILERPLATE(FName, FName)

    // Copy constructor for underlying type
    EXTRA_CONSTRUCTOR_BOILERPLATE(const FName&, FName)

    // Extra constructors for different char types
    CHAR_TYPES_BOILERPLATE(EXTRA_CONSTRUCTOR_BOILERPLATE, FName)

    // Extra assignment operators for different char types
    CHAR_TYPES_BOILERPLATE(EXTRA_ASSIGN_BOILERPLATE, FName)
};

USTRUCT(BlueprintType, meta = (DisplayName = "Text Struct",
                               HasNativeMake = "/Script/GADE7322_TD.CustomStructFunctionLibrary.MakeFTextStruct",
                               HasNativeBreak = "/Script/GADE7322_TD.CustomStructFunctionLibrary.BreakFTextStruct"))
struct GADE7322_TD_API FFTextStruct
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText Value = FText::GetEmpty();

    GENERATE_STRUCT_BOILERPLATE(FText, FText)

    // Copy constructor for underlying type
    EXTRA_CONSTRUCTOR_BOILERPLATE(const FText&, FText)
};

USTRUCT(BlueprintType, meta = (DisplayName = "Object Struct",
                               HasNativeMake = "/Script/GADE7322_TD.CustomStructFunctionLibrary.MakeUObjectStruct",
                               HasNativeBreak = "/Script/GADE7322_TD.CustomStructFunctionLibrary.BreakUObjectStruct"))
struct GADE7322_TD_API FUObjectStruct
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UObject> Value = nullptr;

    GENERATE_STRUCT_BOILERPLATE(TObjectPtr<UObject>, UObject)

    // Extra constructor that takes in UObject*
    EXTRA_CONSTRUCTOR_BOILERPLATE(UObject*, UObject)

    // Idk with these two, will figure out tomorrow
    EXTRA_ASSIGN_BOILERPLATE(TObjectPtr<UObject>, UObject)
    EXTRA_ASSIGN_BOILERPLATE(UObject*, UObject)

    // Extra implicit type operator overload to allow the struct to
    // be implicitly converted to a UObject*
    EXTRA_TYPE_BOILERPLATE(UObject*, return Value, const)
};

#undef CHAR_TYPES_BOILERPLATE
#undef EXTRA_TYPE_BOILERPLATE
#undef EXTRA_ASSIGN_BOILERPLATE
#undef EXTRA_CONSTRUCTOR_BOILERPLATE
#undef EXTRA_BOILERPLATE
#undef GENERATE_STRUCT_BOILERPLATE
#undef STRUCT
