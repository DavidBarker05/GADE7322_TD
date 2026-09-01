// ReSharper disable CppNonExplicitConvertingConstructor
#pragma once

#include "CoreMinimal.h"

#include <type_traits>

#include "CustomStructs.h"
#include "StructUtils/InstancedStruct.h"

#include "Any.generated.h"

// Originally written by me in 2025 as EventData this is the new and improved version which is way more flexible
// https://github.com/DavidBarker05/FirstPersonShooter/blob/master/Source/FirstPersonShooter/Events/EventData.h

// There might be a better way to do this, but I really have no idea and there is nothing online about this.
// I would do something different if Unreal's GC wouldn't get upset like seen in my SDL engine here:
// https://github.com/DavidBarker05/SDL-Game/blob/7b7faec4508d3ad18f7a164791e3fca19f9ba05c/Engine/src/Core/Events/InputEvent.h

// I realised I should probably explain myself since this code is kinda complicated so I'll do my
// best to explain it

// We will be using metaprogramming and specialisation to allow nearly any type to be used with
// FAny. Specialisation, basically just allows you to write a base template and then override that
// for specific types. This allows me to work with more than just USTRUCTs which are technically
// the only type FAny stores, we just convert types to their respective struct found in
// CustomStructs.h

// Metaprogramming, specialisation and SFINAE are all used in this. They're way to complicated to
// be explained by comment but information can be found online. Basically, C++ templates are
// Turing complete and evaluated at compile time and we use the compiler to enable and disable
// certain parts of code based on what we provide in templates

// Partial specialisation for TAnyType, used for USTRUCTS because they don't need a wrapper
// Enable will be used later to only allow certain raw pointers and TObjectPtrs to be used
template<typename T, typename Enable = void>
struct TAnyType
{
    using Type = T; // T is a USTRUCT so Type is just T
};

// Full specialisation for bool
// Basically if bool is ever passed as a template argument to TAnyType it will use this
// instead
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
struct TAnyType<const char*>
{
    using Type = FFStringStruct; // Use FFStringStruct instead of const char*
};

// Full specialisation for char* (basically just array of chars that is
// modifiable)
template<>
struct TAnyType<char*>
{
    using Type = FFStringStruct; // Use FFStringStruct instead of char*
};

// Full specialisation for const WIDECHAR* (basically any string of text
// written like L"Lorem Ipsum" or TEXT("Lorem Ipsum"))
template<>
struct TAnyType<const WIDECHAR*>
{
    using Type = FFStringStruct; // Use FFStringStruct instead of const WIDECHAR*
};

// Full specialisation for WIDECHAR* (basically just array of WIDECHARs that is
// modifiable)
template<>
struct TAnyType<WIDECHAR*>
{
    using Type = FFStringStruct; // Use FFStringStruct instead of WIDECHAR*
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
// TEnableIf only enables the code if the TObjectPtr points to a
// UObject or a type that derives from UObject. Hides it otherwise
template<typename T>
struct TAnyType<T, typename TEnableIf<TIsTObjectPtr_V<T> &&
                                      TIsDerivedFrom<typename TRemoveObjectPointer<T>::Type, UObject>::IsDerived>::Type>
{
    using Type = FUObjectStruct; // Use FUObjectStruct instead of TObjectPtr<...>
};

// Specialisation of ...*
// TEnableIf only enables the code if T = UObject* or what it
// points to derives from UObject. Hides it otherwise
template<typename T>
struct TAnyType<T, typename TEnableIf<TIsPointer<T>::Value &&
                                      TIsDerivedFrom<typename TRemovePointer<T>::Type, UObject>::IsDerived>::Type>
{
    using Type = FUObjectStruct; // Use FUObjectStruct instead of ...*
};

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

    // Copy constructor. Stores InValue in Value. Uses T if it's as struct or the struct
    // equivalent if not
    template<typename T>
    FAny(const T& InValue) : Value(FInstancedStruct::Make<typename TAnyType<T>::Type>(InValue))
    {
    }

    // Move constructor. Moves InValue into Value. Uses T if it's as struct or the struct
    // equivalent if not. Don't enable if T is FAny
    template<typename T, TEnableIf<!std::is_same_v<TDecay<T>, FAny>>>
    FAny(T&& InValue) : Value(FInstancedStruct::Make<typename TAnyType<TDecay<T>>::Type>(Forward<T>(InValue)))
    {
    }

    // Forwarding constructor. Makes Value store Type if it is a struct, or the struct
    // equivalent if not. Forwards the params into the constructor for the struct
    template<typename Type, typename... Args>
    FAny(Args&&... Params) : Value(FInstancedStruct::Make<typename TAnyType<Type>::Type>(Forward<Args>(Params)...))
    {
    }

    FAny& operator=(const FAny&) = default;
    FAny& operator=(FAny&&) noexcept = default;

    FAny& operator=(const FInstancedStruct& InValue)
    {
        Value = InValue;
        return *this;
    }

    // Copy assignment operator. See copy constructor for how it works
    template<typename T>
    FAny& operator=(const T& InValue)
    {
        Value = FInstancedStruct::Make<typename TAnyType<T>::Type>(InValue);
        return *this;
    }

    // Move assignment operator. See move constructor for how it works
    template<typename T, TEnableIf<!std::is_same_v<TDecay<T>, FAny>>>
    FAny& operator=(T&& InValue)
    {
        Value = FInstancedStruct::Make<typename TAnyType<TDecay<T>>::Type>(Forward<T>(InValue));
        return *this;
    }

    bool IsValid() const { return Value.IsValid(); }

    // Get a mutable pointer to the underlying data
    template<typename T>
    T* Get()
    {
        using StoredType = TAnyType<T>::Type; // The struct type
        if constexpr (std::is_same_v<StoredType, FUObjectStruct>) // Data is UObject ptr
        {
            FUObjectStruct* Wrapper = Value.GetMutablePtr<FUObjectStruct>();
            if (!Wrapper) return nullptr; // Value does not hold a FUObjectStruct
            if constexpr (TIsTObjectPtr_V<T>) // TObjectPtr<...>
            {
                using PointedType = TRemoveObjectPointer<T>::Type; // The ...
                if (!Cast<PointedType>(Wrapper->Value))
                    return nullptr; // If can't cast to PointedType return nullptr
            }
            else // ...*
            {
                using PointedType = TRemovePointer<T>::Type; // ...
                if (!Cast<PointedType>(Wrapper->Value.Get()))
                    return nullptr; // If can't cast to PointedType return nullptr
            }
            return reinterpret_cast<T*>(&Wrapper->Value); // Reinterpret the address to the underlying data as T* and return it
        }
        else if constexpr (std::is_same_v<T, StoredType>) return Value.GetMutablePtr<T>(); // T is a USTRUCT
        else
        {
            StoredType* Wrapper = Value.GetMutablePtr<StoredType>();
            return Wrapper ? &Wrapper->Value : nullptr; // T was wrapped (e.g. int32 -> FInt32Struct)
        }
    }

    // Get an immutable pointer to the underlying data
    template<typename T>
    const T* Get() const
    {
        // See the mutable Get for how this works. It's the same
        using StoredType = TAnyType<T>::Type;
        if constexpr (std::is_same_v<StoredType, FUObjectStruct>)
        {
            const FUObjectStruct* Wrapper = Value.GetPtr<FUObjectStruct>();
            if (!Wrapper) return nullptr;
            if constexpr (TIsTObjectPtr_V<T>)
            {
                using PointedType = TRemoveObjectPointer<T>::Type;
                if (!Cast<PointedType>(Wrapper->Value)) return nullptr;
            }
            else
            {
                using PointedType = TRemovePointer<T>::Type;
                if (!Cast<PointedType>(Wrapper->Value.Get())) return nullptr;
            }
            return reinterpret_cast<const T*>(&Wrapper->Value);
        }
        else if constexpr (std::is_same_v<T, StoredType>) return Value.GetPtr<T>();
        else
        {
            const StoredType* Wrapper = Value.GetPtr<StoredType>();
            return Wrapper ? &Wrapper->Value : nullptr;
        }
    }

    // Copy Set. See copy constructor for how it works
    template<typename T>
    FAny& Set(const T& InValue)
    {
        Value = FInstancedStruct::Make<typename TAnyType<T>::Type>(InValue);
        return *this;
    }

    // Move Set. See move constructor for how it works
    template<typename T>
    FAny& Set(T&& InValue)
    {
        Value = FInstancedStruct::Make<typename TAnyType<TDecay<T>>::Type>(Forward<T>(InValue));
        return *this;
    }
};
