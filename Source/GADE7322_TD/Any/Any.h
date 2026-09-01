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

template<typename T, typename Enable = void>
struct TAnyType
{
    using Type = T;
};

template<>
struct TAnyType<bool>
{
    using Type = FBoolStruct;
};

template<>
struct TAnyType<uint8>
{
    using Type = FUint8Struct;
};

template<>
struct TAnyType<int32>
{
    using Type = FInt32Struct;
};

template<>
struct TAnyType<int64>
{
    using Type = FInt64Struct;
};

template<>
struct TAnyType<float>
{
    using Type = FFloatStruct;
};

template<>
struct TAnyType<FString>
{
    using Type = FFStringStruct;
};

template<>
struct TAnyType<const char*>
{
    using Type = FFStringStruct;
};

template<>
struct TAnyType<char*>
{
    using Type = FFStringStruct;
};

template<>
struct TAnyType<const WIDECHAR*>
{
    using Type = FFStringStruct;
};

template<>
struct TAnyType<WIDECHAR*>
{
    using Type = FFStringStruct;
};

template<>
struct TAnyType<FName>
{
    using Type = FFNameStruct;
};

template<>
struct TAnyType<FText>
{
    using Type = FFTextStruct;
};

template<typename T>
struct TAnyType<T, typename TEnableIf<TIsTObjectPtr_V<T> &&
                                      TIsDerivedFrom<typename TRemoveObjectPointer<T>::Type, UObject>::IsDerived>::Type>
{
    using Type = FUObjectStruct;
};

template<typename T>
struct TAnyType<T, typename TEnableIf<TIsPointer<T>::Value &&
                                      TIsDerivedFrom<typename TRemovePointer<T>::Type, UObject>::IsDerived>::Type>
{
    using Type = FUObjectStruct;
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

    FAny() = default;
    FAny(const FAny&) = default;
    FAny(FAny&&) noexcept = default;

    FAny(const FInstancedStruct& InValue) : Value(InValue) { }

    FAny(FInstancedStruct&& InValue) : Value(MoveTemp(InValue)) { }

    template<typename T>
    FAny(const T& InValue) : Value(FInstancedStruct::Make<typename TAnyType<T>::Type>(InValue))
    {
    }

    template<typename T, TEnableIf<!std::is_same_v<TDecay<T>, FAny>>>
    FAny(T&& InValue) : Value(FInstancedStruct::Make<typename TAnyType<TDecay<T>>::Type>(Forward<T>(InValue)))
    {
    }

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

    template<typename T>
    FAny& operator=(const T& InValue)
    {
        Value = FInstancedStruct::Make<typename TAnyType<T>::Type>(InValue);
        return *this;
    }

    template<typename T, TEnableIf<!std::is_same_v<TDecay<T>, FAny>>>
    FAny& operator=(T&& InValue)
    {
        Value = FInstancedStruct::Make<typename TAnyType<TDecay<T>>::Type>(Forward<T>(InValue));
        return *this;
    }

    bool IsValid() const { return Value.IsValid(); }

    template<typename T>
    T* Get()
    {
        using StoredType = TAnyType<T>::Type;
        if constexpr (std::is_same_v<StoredType, FUObjectStruct>)
        {
            FUObjectStruct* Wrapper = Value.GetMutablePtr<FUObjectStruct>();
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
            return reinterpret_cast<T*>(&Wrapper->Value);
        }
        else if constexpr (std::is_same_v<T, StoredType>) return Value.GetMutablePtr<T>(); // T is a USTRUCT
        else
        {
            StoredType* Wrapper = Value.GetMutablePtr<StoredType>();
            return Wrapper ? &Wrapper->Value : nullptr; // T was wrapped (e.g. int32 -> FInt32Struct)
        }
    }

    template<typename T>
    const T* Get() const
    {
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

    template<typename T>
    FAny& Set(const T& InValue)
    {
        Value = FInstancedStruct::Make<typename TAnyType<T>::Type>(InValue);
        return *this;
    }

    template<typename T>
    FAny& Set(T&& InValue)
    {
        Value = FInstancedStruct::Make<typename TAnyType<TDecay<T>>::Type>(Forward<T>(InValue));
        return *this;
    }
};
