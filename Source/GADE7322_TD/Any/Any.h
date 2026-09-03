// ReSharper disable CppNonExplicitConvertingConstructor
#pragma once

#include "CoreMinimal.h"

#include "CustomStructs.h"
#include "StructUtils/InstancedStruct.h"
#include "TDTypeTraits.h"

#include "Any.generated.h"

// Originally written by me in 2025 as EventData this is the new and improved version which is way more flexible
// https://github.com/DavidBarker05/FirstPersonShooter/blob/master/Source/FirstPersonShooter/Events/EventData.h

// There might be a better way to do this, but I really have no idea and there is nothing online about this.
// I would do something different if Unreal's GC wouldn't get upset like seen in my SDL engine here:
// https://github.com/DavidBarker05/SDL-Game/blob/7b7faec4508d3ad18f7a164791e3fca19f9ba05c/Engine/src/Core/Events/InputEvent.h

// I realised I should probably explain myself since this code is kinda complicated, so I'll do my best to explain it

// We will be using metaprogramming and specialisation to allow nearly any type to be used with FAny. Specialisation,
// basically just allows you to write a base template and then override that for specific types. This allows me to work
// with more than just USTRUCTs which are technically the only type FAny stores, we just convert types to their
// respective struct found in CustomStructs.h

// Metaprogramming, specialisation and the requires clause (introduced in C++20) are all used in this. They're way too
// complicated to be explained by comment but information can be found online. Basically, C++ templates are Turing
// complete and evaluated at compile time, and we use the compiler to enable and disable certain parts of code based on
// what we provide in templates

// Partial specialisation for TAnyType, used for USTRUCTS because they don't need a wrapper
template<typename T>
struct TAnyType
{
    using Type = T; // T is a USTRUCT so Type is just T
};

// Full specialisation for bool
// Basically if bool is ever passed as a template argument to TAnyType it will use this instead
template<>
struct TAnyType<bool>
{
    using Type = FBoolStruct; // Use FBoolStruct instead of bool
};

// Full specialisation for uint8
template<>
struct TAnyType<uint8>
{
    using Type = FUint8Struct; // Use FUint8Struct instead of uint8
};

// Full specialisation for int32
template<>
struct TAnyType<int32>
{
    using Type = FInt32Struct; // Use FInt32Struct instead of int32
};

// Full specialisation for int64
template<>
struct TAnyType<int64>
{
    using Type = FInt64Struct; // Use FInt64Struct instead of int64
};

// Full specialisation for float
template<>
struct TAnyType<float>
{
    using Type = FFloatStruct; // Use FFloatStruct instead of float
};

// Full specialisation for FString
template<>
struct TAnyType<FString>
{
    using Type = FFStringStruct; // Use FFStringStruct instead of FString
};

// Full specialisation for const char* (basically any string text written like
// "Lorem Ipsum")
template<>
struct TAnyType<const ANSICHAR*>
{
    using Type = FFStringStruct; // Use FFStringStruct instead of const char*
};

// Full specialisation for const WIDECHAR* (basically any string of text
// written like L"Lorem Ipsum" or TEXT("Lorem Ipsum"))
template<>
struct TAnyType<const WIDECHAR*>
{
    using Type = FFStringStruct; // Use FFStringStruct instead of const WIDECHAR*
};

// Full specialisation for const UTF8CHAR* (basically any string of text
// written like u8"Lorem Ipsum")
template<>
struct TAnyType<const UTF8CHAR*>
{
    using Type = FFStringStruct; // Use FFStringStruct instead of const UTF8CHAR*
};

// Full specialisation for const UCS2CHAR* (basically any string of text
// written like u16"Lorem Ipsum")
template<>
struct TAnyType<const UCS2CHAR*>
{
    using Type = FFStringStruct; // Use FFStringStruct instead of const UCS2CHAR*
};

// Full specialisation for const UTF32CHAR* (basically any string of text
// written like u32"Lorem Ipsum")
template<>
struct TAnyType<const UTF32CHAR*>
{
    using Type = FFStringStruct; // Use FFStringStruct instead of const UTF32CHAR*
};

// Full specialisation for FName
template<>
struct TAnyType<FName>
{
    using Type = FFNameStruct; // Use FFNameStruct instead of FName
};

// Full specialisation for FText
template<>
struct TAnyType<FText>
{
    using Type = FFTextStruct; // Use FFTextStruct instead of FText
};

// Specialisation for T = TObjectPtr<...>
// Requires ... to derive from UObject
template<typename T>
requires TIsTObjectPtr_V<T> && TIsDerivedFrom_V<TRemoveObjectPointer_T<T>, UObject>
struct TAnyType<T>
{
    using Type = FUObjectStruct; // Use FUObjectStruct instead of TObjectPtr<...>
};

// Specialisation of ...*
// Requires ... to derive from UObject
template<typename T>
requires TIsPointer_V<T> && TIsDerivedFrom_V<TRemovePointer_T<T>, UObject>
struct TAnyType<T>
{
    using Type = FUObjectStruct; // Use FUObjectStruct instead of ...*
};

template<typename T>
using TAnyType_T = typename TAnyType<T>::Type;

/*
  Store any type inside similar to std::any in C++ standard library. This allows to store and pass around
  virtually any type you need and can be used between C++ and blueprints

  Can Store:
    - bool [FBoolStruct]
    - uint8 [FUint8Struct]
    - int32 [FInt32Struct]
    - int64 [FInt64Struct]
    - float [FFloatStruct]
    - FString [FFStringStruct]
    - FName [FFNameStruct]
    - FText [FFTextStruct]
    - UObject* or TObjectPtr<UObject> and any class that derives from UObject [FUObjectStruct]
    - Structs recognised by Unreal (obviously)

  Note: Doesn't store collections like arrays because uses structs to emulate std::any and making a struct
  for each array type was not worth it. Also, TArray<AActor*> != TArray<UObject*> so that would mean either
  a new struct for each subclass of every Unreal object or creating an array of the base class and copying
  each element over one at a time every time. If needed they can be added easily
 */
USTRUCT(BlueprintType, meta = (HasNativeMake = "/Script/GADE7322_TD.AnyFunctionLibrary.MakeAny"))
struct GADE7322_TD_API FAny
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FInstancedStruct Value;
    // ^ FInstancedStruct stores any USTRUCT

    FAny() = default;
    FAny(const FAny&) = default;
    FAny(FAny&&) noexcept = default;

    FAny(const FInstancedStruct& InValue) : Value(InValue) { }

    FAny(FInstancedStruct&& InValue) : Value(MoveTemp(InValue)) { }

    // Forwarding constructor. Constructs FAny by perfectly forwarding InValue into Value, wrapping it as T (or the
    // struct-equivalent of T if T isn't already a struct)
    template<typename T>
    requires(!TIsSame_V<TDecay_T<T>, FAny>) // T can't be variation of FAny
    FAny(T&& InValue) : Value(FInstancedStruct::Make<TAnyType_T<TDecay_T<T>>>(Forward<T>(InValue)))
    {
    }

    // In-place (emplace) constructor. Constructs the wrapped value as Type if it is a struct or its struct-equivalent,
    // directly from the forwarded Params, without requiring an existing Type instance to move or copy from
    template<typename Type, typename... Args>
    FAny(Args&&... Params) : Value(FInstancedStruct::Make<TAnyType_T<Type>>(Forward<Args>(Params)...))
    {
    }

    FAny& operator=(const FAny&) = default;
    FAny& operator=(FAny&&) noexcept = default;

    FAny& operator=(const FInstancedStruct& InValue)
    {
        Value = InValue;
        return *this;
    }

    // Forwarding assignment operator. Assigns to FAny by perfectly forwarding InValue into Value, wrapping it as T (or
    // the struct-equivalent of T if T isn't already a struct)
    template<typename T>
    requires(!TIsSame_V<TDecay_T<T>, FAny>) // T can't be variation of FAny
    FAny& operator=(T&& InValue)
    {
        Value = FInstancedStruct::Make<TAnyType_T<TDecay_T<T>>>(Forward<T>(InValue));
        return *this;
    }

    bool IsValid() const { return Value.IsValid(); }

    // Get a mutable pointer to the underlying data as a TObjectPtr<...> that derives from UObject
    template<typename T>
    requires TIsSame_V<TAnyType_T<T>, FUObjectStruct> && TIsTObjectPtr_V<T>
    T* Get()
    {
        FUObjectStruct* Wrapper = Value.GetMutablePtr<FUObjectStruct>();
        if (!Wrapper) return nullptr; // Value doesn't hold an FUObjectStruct
        using PointedType = TRemoveObjectPointer_T<T>;
        if (!Cast<PointedType>(Wrapper->Value)) return nullptr; // Can't cast to desired type
        return reinterpret_cast<T*>(&Wrapper->Value);
    }

    // Get a mutable pointer to the underlying data as a raw pointer that derives from UObject
    template<typename T>
    requires TIsSame_V<TAnyType_T<T>, FUObjectStruct> && TIsPointer_V<T>
    T* Get()
    {
        FUObjectStruct* Wrapper = Value.GetMutablePtr<FUObjectStruct>();
        if (!Wrapper) return nullptr; // Value doesn't hold an FUObjectStruct
        using PointedType = TRemovePointer_T<T>;
        if (!Cast<PointedType>(Wrapper->Value)) return nullptr; // Can't cast to desired type
        return reinterpret_cast<T*>(&Wrapper->Value);
    }

    // Get a mutable pointer to the underlying data as a USTRUCT
    template<typename T>
    requires TIsSame_V<TAnyType_T<T>, T> // Only same if T is a USTRUCT
    T* Get()
    {
        return Value.GetMutablePtr<T>();
    }

    // Get a mutable pointer to the underlying data as a non-UObject or USTRUCT
    template<typename T>
    requires(!TIsSame_V<TAnyType_T<T>, T> && !TIsSame_V<TAnyType_T<T>, FUObjectStruct>)
    T* Get()
    {
        using StoredType = TAnyType_T<T>;
        StoredType* Wrapper = Value.GetMutablePtr<StoredType>();
        return Wrapper ? &Wrapper->Value : nullptr;
    }

    // Get an immutable pointer to the underlying data as a TObjectPtr<...> that derives from UObject
    template<typename T>
    requires TIsSame_V<TAnyType_T<T>, FUObjectStruct> && TIsTObjectPtr_V<T>
    const T* Get() const
    {
        const FUObjectStruct* Wrapper = Value.GetPtr<FUObjectStruct>();
        if (!Wrapper) return nullptr; // Value doesn't hold an FUObjectStruct
        using PointedType = TRemoveObjectPointer_T<T>;
        if (!Cast<PointedType>(Wrapper->Value)) return nullptr; // Can't cast to desired type
        return reinterpret_cast<const T*>(&Wrapper->Value);
    }

    // Get an immutable pointer to the underlying data as a raw pointer that derives from UObject
    template<typename T>
    requires TIsSame_V<TAnyType_T<T>, FUObjectStruct> && TIsPointer_V<T>
    const T* Get() const
    {
        const FUObjectStruct* Wrapper = Value.GetPtr<FUObjectStruct>();
        if (!Wrapper) return nullptr; // Value doesn't hold an FUObjectStruct
        using PointedType = TRemovePointer_T<T>;
        if (!Cast<PointedType>(Wrapper->Value)) return nullptr; // Can't cast to desired type
        return reinterpret_cast<const T*>(&Wrapper->Value);
    }

    // Get an immutable pointer to the underlying data as a USTRUCT
    template<typename T>
    requires TIsSame_V<TAnyType_T<T>, T> // Only same if T is a USTRUCT
    const T* Get() const
    {
        return Value.GetPtr<T>();
    }

    // Get an immutable pointer to the underlying data as a non-UObject or USTRUCT
    template<typename T>
    requires(!TIsSame_V<TAnyType_T<T>, T> && !TIsSame_V<TAnyType_T<T>, FUObjectStruct>)
    const T* Get() const
    {
        using StoredType = TAnyType_T<T>;
        const StoredType* Wrapper = Value.GetPtr<StoredType>();
        return Wrapper ? &Wrapper->Value : nullptr;
    }

    // Forwarding Set. Sets the value of FAny by perfectly forwarding InValue into Value, wrapping it as T (or the
    // struct-equivalent of T if T isn't already a struct)
    template<typename T>
    FAny& Set(T&& InValue)
    {
        Value = FInstancedStruct::Make<TAnyType_T<TDecay_T<T>>>(Forward<T>(InValue));
        return *this;
    }
};
