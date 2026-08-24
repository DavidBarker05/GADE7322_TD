#include "Any/CustomStructFunctionLibrary.h"

#include "Templates/SubclassOf.h"

// FBoolStruct
FBoolStruct UCustomStructFunctionLibrary::MakeBoolStruct(bool InValue) { return FBoolStruct(InValue); }

bool UCustomStructFunctionLibrary::BreakBoolStruct(const FBoolStruct& BooleanStruct) { return BooleanStruct.Value; }

bool UCustomStructFunctionLibrary::FBoolStruct_To_Bool(const FBoolStruct& A) { return A.Value; }

FBoolStruct UCustomStructFunctionLibrary::Bool_ToFBoolStruct(bool A) { return FBoolStruct(A); }

bool UCustomStructFunctionLibrary::FBoolStruct_Get(const FBoolStruct& InStruct) { return InStruct.Value; }

FBoolStruct& UCustomStructFunctionLibrary::FBoolStruct_Set(FBoolStruct& InStruct, bool NewValue)
{
    return InStruct.Set(NewValue);
}

// FUint8Struct
FUint8Struct UCustomStructFunctionLibrary::MakeUint8Struct(uint8 InValue) { return FUint8Struct(InValue); }

uint8 UCustomStructFunctionLibrary::BreakUint8Struct(const FUint8Struct& ByteStruct) { return ByteStruct.Value; }

uint8 UCustomStructFunctionLibrary::FUint8Struct_To_Uint8(const FUint8Struct& A) { return A.Value; }

FUint8Struct UCustomStructFunctionLibrary::Uint8_To_FUint8Struct(uint8 A) { return FUint8Struct(A); }

uint8 UCustomStructFunctionLibrary::FUint8Struct_Get(const FUint8Struct& InStruct) { return InStruct.Value; }

FUint8Struct& UCustomStructFunctionLibrary::FUint8Struct_Set(FUint8Struct& InStruct, uint8 NewValue)
{
    return InStruct.Set(NewValue);
}

// FInt32Struct
FInt32Struct UCustomStructFunctionLibrary::MakeInt32Struct(int32 InValue) { return FInt32Struct(InValue); }

int32 UCustomStructFunctionLibrary::BreakInt32Struct(const FInt32Struct& IntegerStruct) { return IntegerStruct.Value; }

int32 UCustomStructFunctionLibrary::FInt32Struct_To_Int32(const FInt32Struct& A) { return A.Value; }

FInt32Struct UCustomStructFunctionLibrary::Int32_To_FInt32Struct(int32 A) { return FInt32Struct(A); }

int32 UCustomStructFunctionLibrary::FInt32Struct_Get(const FInt32Struct& InStruct) { return InStruct.Value; }

FInt32Struct& UCustomStructFunctionLibrary::FInt32Struct_Set(FInt32Struct& InStruct, int32 NewValue)
{
    return InStruct.Set(NewValue);
}

// FInt64Struct
FInt64Struct UCustomStructFunctionLibrary::MakeInt64Struct(int64 InValue) { return FInt64Struct(InValue); }

int64 UCustomStructFunctionLibrary::BreakInt64Struct(const FInt64Struct& Integer64Struct)
{
    return Integer64Struct.Value;
}

int64 UCustomStructFunctionLibrary::FInt64Struct_To_Int64(const FInt64Struct& A) { return A.Value; }

FInt64Struct UCustomStructFunctionLibrary::Int64_To_FInt64Struct(int64 A) { return FInt64Struct(A); }

int64 UCustomStructFunctionLibrary::FInt64Struct_Get(const FInt64Struct& InStruct) { return InStruct.Value; }

FInt64Struct& UCustomStructFunctionLibrary::FInt64Struct_Set(FInt64Struct& InStruct, int64 NewValue)
{
    return InStruct.Set(NewValue);
}

// FFloatStruct
FFloatStruct UCustomStructFunctionLibrary::MakeFloatStruct(float InValue) { return FFloatStruct(InValue); }

float UCustomStructFunctionLibrary::BreakFloatStruct(const FFloatStruct& FloatStruct) { return FloatStruct.Value; }

float UCustomStructFunctionLibrary::FFloatStruct_To_Float(const FFloatStruct& A) { return A.Value; }

FFloatStruct UCustomStructFunctionLibrary::Float_To_FFloatStruct(float A) { return FFloatStruct(A); }

float UCustomStructFunctionLibrary::FFloatStruct_Get(const FFloatStruct& InStruct) { return InStruct.Value; }

FFloatStruct& UCustomStructFunctionLibrary::FFloatStruct_Set(FFloatStruct& InStruct, float NewValue)
{
    return InStruct.Set(NewValue);
}

FFStringStruct UCustomStructFunctionLibrary::MakeFStringStruct(const FString& InValue)
{
    return FFStringStruct(InValue);
}

FString UCustomStructFunctionLibrary::BreakFStringStruct(const FFStringStruct& StringStruct)
{
    return StringStruct.Value;
}

// FFStringStruct
FString UCustomStructFunctionLibrary::FFStringStruct_To_FString(const FFStringStruct& A) { return A.Value; }

FFStringStruct UCustomStructFunctionLibrary::FString_To_FFStringStruct(const FString& A) { return FFStringStruct(A); }

FString UCustomStructFunctionLibrary::FFStringStruct_Get(const FFStringStruct& InStruct) { return InStruct.Value; }

FFStringStruct& UCustomStructFunctionLibrary::FFStringStruct_Set(FFStringStruct& InStruct, const FString& NewValue)
{
    return InStruct.Set(NewValue);
}

// FFNameStruct
FFNameStruct UCustomStructFunctionLibrary::MakeFNameStruct(const FName& InValue) { return FFNameStruct(InValue); }

FName UCustomStructFunctionLibrary::BreakFNameStruct(const FFNameStruct& NameStruct) { return NameStruct.Value; }

FName UCustomStructFunctionLibrary::FFNameStruct_To_FName(const FFNameStruct& A) { return A.Value; }

FFNameStruct UCustomStructFunctionLibrary::FName_To_FFNameStruct(const FName& A) { return FFNameStruct(A); }

FName UCustomStructFunctionLibrary::FFNameStruct_Get(const FFNameStruct& InStruct) { return InStruct.Value; }

FFNameStruct& UCustomStructFunctionLibrary::FFNameStruct_Set(FFNameStruct& InStruct, const FName& NewValue)
{
    return InStruct.Set(NewValue);
}

// FFTextStruct
FFTextStruct UCustomStructFunctionLibrary::MakeFTextStruct(const FText& InValue) { return FFTextStruct(InValue); }

FText UCustomStructFunctionLibrary::BreakFTextStruct(const FFTextStruct& TextStruct) { return TextStruct.Value; }

FText UCustomStructFunctionLibrary::FFTextStruct_To_FText(const FFTextStruct& A) { return A.Value; }

FFTextStruct UCustomStructFunctionLibrary::FText_ToFFTextStruct(const FText& A) { return FFTextStruct(A); }

FText UCustomStructFunctionLibrary::FFTextStruct_Get(const FFTextStruct& InStruct) { return InStruct.Value; }

FFTextStruct& UCustomStructFunctionLibrary::FFTextStruct_Set(FFTextStruct& InStruct, const FText& NewValue)
{
    return InStruct.Set(NewValue);
}

// FUObjectStruct
FUObjectStruct UCustomStructFunctionLibrary::MakeUObjectStruct(UObject* InValue) { return FUObjectStruct(InValue); }

UObject* UCustomStructFunctionLibrary::BreakUObjectStruct(const FUObjectStruct& ObjectStruct)
{
    return ObjectStruct.Value;
}

UObject* UCustomStructFunctionLibrary::FUObjectStruct_To_UObject(const FUObjectStruct& A) { return A.Value; }

FUObjectStruct UCustomStructFunctionLibrary::UObject_To_FUObjectStruct(UObject* A) { return FUObjectStruct(A); }

UObject* UCustomStructFunctionLibrary::FUObjectStruct_Get(const FUObjectStruct& InStruct) { return InStruct.Value; }

UObject* UCustomStructFunctionLibrary::FUObjectStruct_GetAsType(const FUObjectStruct& ObjectStruct,
                                                                TSubclassOf<UObject> Class, EIsAOutputPins& OutputPins)
{
    if (ObjectStruct.Get() && ObjectStruct.Get().IsA(Class.Get()->StaticClass()))
    {
        OutputPins = EIsAOutputPins::IsType;
        return ObjectStruct;
    }
    OutputPins = EIsAOutputPins::IsNotType;
    return nullptr;
}

FUObjectStruct& UCustomStructFunctionLibrary::FUObjectStruct_Set(FUObjectStruct& InStruct, UObject* NewValue)
{
    return InStruct.Set(NewValue);
}
