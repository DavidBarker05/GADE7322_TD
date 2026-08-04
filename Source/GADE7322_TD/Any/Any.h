#pragma once

#include "CoreMinimal.h"

#include "StructUtils/InstancedStruct.h"
#include "CustomStructs.h"

#include "Any.generated.h"

// Originally written by me in 2025 as EventData this is the new and improved version which is way more flexible
// https://github.com/DavidBarker05/FirstPersonShooter/blob/master/Source/FirstPersonShooter/Events/EventData.h

#ifndef COPY_CONSTRUCTOR_GENERATE
#define COPY_CONSTRUCTOR_GENERATE(Type, Struct) \
    FAny(const Type& InValue) : Value(FInstancedStruct::Make<Struct>(InValue)) \
    { \
    }

#define COPY_CONSTRUCTOR_GENERATE_PTR(Type, Struct) \
    FAny(Type* const& InValue) : Value(FInstancedStruct::Make<Struct>(InValue)) \
    { \
    }

#define COPY_CONSTRUCTOR_GENERATE_TOBJECTPTR(Type, Struct) \
    FAny(const TObjectPtr<Type>& InValue) : Value(FInstancedStruct::Make<Struct>(InValue)) \
    { \
    }
#endif

#ifndef MOVE_CONSTRUCTOR_GENERATE
#define MOVE_CONSTRUCTOR_GENERATE(Type, Struct) \
    FAny(Type&& InValue) : Value(FInstancedStruct::Make<Struct>(MoveTemp(InValue))) \
    { \
    }

#define MOVE_CONSTRUCTOR_GENERATE_PTR(Type, Struct) \
    FAny(Type*&& InValue) : Value(FInstancedStruct::Make<Struct>(MoveTemp(InValue))) \
    { \
    }

#define MOVE_CONSTRUCTOR_GENERATE_TOBJECTPTR(Type, Struct) \
    FAny(TObjectPtr<Type>&& InValue) : Value(FInstancedStruct::Make<Struct>(MoveTemp(InValue))) \
    { \
    }
#endif

#ifndef COPY_ASSIGN_GENERATE
#define COPY_ASSIGN_GENERATE(Type, Struct) \
    inline const FAny& operator=(const Type& InValue) \
    { \
        Value = FInstancedStruct::Make<Struct>(InValue); \
        return *this; \
    }

#define COPY_ASSIGN_GENERATE_PTR(Type, Struct) \
    inline const FAny& operator=(Type* const& InValue) \
    { \
        Value = FInstancedStruct::Make<Struct>(InValue); \
        return *this; \
    }

#define COPY_ASSIGN_GENERATE_TOBJECTPTR(Type, Struct) \
    inline const FAny& operator=(const TObjectPtr<Type>& InValue) \
    { \
        Value = FInstancedStruct::Make<Struct>(InValue); \
        return *this; \
    }
#endif

#ifndef MOVE_ASSIGN_GENERATE
#define MOVE_ASSIGN_GENERATE(Type, Struct) \
    inline const FAny& operator=(Type&& InValue) \
    { \
        Value = FInstancedStruct::Make<Struct>(MoveTemp(InValue)); \
        return *this; \
    }

#define MOVE_ASSIGN_GENERATE_PTR(Type, Struct) \
    inline const FAny& operator=(Type*&& InValue) \
    { \
        Value = FInstancedStruct::Make<Struct>(MoveTemp(InValue)); \
        return *this; \
    }

#define MOVE_ASSIGN_GENERATE_TOBJECTPTR(Type, Struct) \
    inline const FAny& operator=(TObjectPtr<Type>&& InValue) \
    { \
        Value = FInstancedStruct::Make<Struct>(MoveTemp(InValue)); \
        return *this; \
    }
#endif

#ifndef GET_GENERATE
#define GET_GENERATE(Type, Struct) \
    template<> \
    inline Type* FAny::Get<Type>() \
    { \
        if (Struct* StructPtr = Value.GetMutablePtr<Struct>()) \
        { \
            return &StructPtr->Value; \
        } \
        return nullptr; \
    } \
    template<> \
    inline const Type* FAny::Get<Type>() const \
    { \
        if (const Struct* StructPtr = Value.GetPtr<Struct>()) \
        { \
            return &StructPtr->Value; \
        } \
        return nullptr; \
    }

#define GET_GENERATE_PTR(Type, Struct) \
    template<> \
    inline Type** FAny::Get<Type*>() \
    { \
        if (Struct* StructPtr = Value.GetMutablePtr<Struct>()) \
        { \
            return reinterpret_cast<Type**>(&StructPtr->Value); \
        } \
        return nullptr; \
    } \
    template<> \
    inline Type* const* FAny::Get<Type*>() const \
    { \
        if (const Struct* StructPtr = Value.GetPtr<Struct>()) \
        { \
            return reinterpret_cast<Type* const*>(&StructPtr->Value); \
        } \
        return nullptr; \
    }

#define GET_GENERATE_TOBJECTPTR(Type, Struct) \
    template<> \
    inline TObjectPtr<Type>* FAny::Get<TObjectPtr<Type>>() \
    { \
        if (Struct* StructPtr = Value.GetMutablePtr<Struct>()) \
        { \
            return &StructPtr->Value; \
        } \
        return nullptr; \
    } \
    template<> \
    inline const TObjectPtr<Type>* FAny::Get<TObjectPtr<Type>>() const \
    { \
        if (const Struct* StructPtr = Value.GetPtr<Struct>()) \
        { \
            return &StructPtr->Value; \
        } \
        return nullptr; \
    }
#endif

#ifndef COPY_SET_GENERATE
#define COPY_SET_GENERATE(Type, Struct) \
    template<> \
    inline FAny& FAny::Set<Type>(const Type& InValue) \
    { \
        Value = FInstancedStruct::Make<Struct>(InValue); \
        return *this; \
    }

#define COPY_SET_GENERATE_PTR(Type, Struct) \
    template<> \
    inline FAny& FAny::Set<Type*>(Type* const& InValue) \
    { \
        Value = FInstancedStruct::Make<Struct>(InValue); \
        return *this; \
    }

#define COPY_SET_GENERATE_TOBJECTPTR(Type, Struct) \
    template<> \
    inline FAny& FAny::Set<TObjectPtr<Type>>(const TObjectPtr<Type>& InValue) \
    { \
        Value = FInstancedStruct::Make<Struct>(InValue); \
        return *this; \
    }
#endif

#ifndef MOVE_SET_GENERATE
#define MOVE_SET_GENERATE(Type, Struct) \
    template<> \
    inline FAny& FAny::Set<Type>(Type&& InValue) \
    { \
        Value = FInstancedStruct::Make<Struct>(MoveTemp(InValue)); \
        return *this; \
    }

#define MOVE_SET_GENERATE_PTR(Type, Struct) \
    template<> \
    inline FAny& FAny::Set<Type*>(Type*&& InValue) \
    { \
        Value = FInstancedStruct::Make<Struct>(MoveTemp(InValue)); \
        return *this; \
    }

#define MOVE_SET_GENERATE_TOBJECTPTR(Type, Struct) \
    template<> \
    inline FAny& FAny::Set<TObjectPtr<Type>>(TObjectPtr<Type>&& InValue) \
    { \
        Value = FInstancedStruct::Make<Struct>(MoveTemp(InValue)); \
        return *this; \
    }
#endif

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
  for each array type was not worth it. Also TArray<AActor*> != TArray<UObject*> so that would mean either
  a new struct for each subclass of every Unreal object or creating an array of the base class and copying
  each element over one at a time every time. If needed they can be added easily by just using the macros
  
  There might be a better way to do this, but I really have no idea and there is nothing online about this.
  I would do something different if Unreal's GC wouldn't get upset like seen in my SDL engine here:
  https://github.com/DavidBarker05/SDL-Game/blob/7b7faec4508d3ad18f7a164791e3fca19f9ba05c/Engine/src/Core/Events/InputEvent.h
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

    FAny(const FInstancedStruct& InValue) : Value(InValue)
    {
    }

    FAny(FInstancedStruct&& InValue) : Value(MoveTemp(InValue))
    {
    }

    template<typename T>
    FAny(const T& InValue)
    {
        if constexpr (TIsTObjectPtr<T>::Value)
        {
            using ObjectType = TRemoveObjectPointer<T>::Type;
            static_assert(TIsDerivedFrom<ObjectType, UObject>::IsDerived,
                          "FAny doesn't work with non-UObject derived TObjectPtrs");
            Value = FInstancedStruct::Make<FUObjectStruct>(InValue);
        }
        else if constexpr (TIsPointer<T>::Value)
        {
            using PointedType = TRemovePointer<T>::Type;
            if constexpr (TIsPointer<PointedType>::Value)
            {
                using PointedPointedType = TRemovePointer<PointedType>::Type; // Can't think of better name
                static_assert(TIsDerivedFrom<PointedType, UObject>::IsDerived,
                              "FAny doesn't work with non-UObject derived pointers to pointers");
                Value = FInstancedStruct::Make<FUObjectStruct>(*InValue);
            }
            else
            {
                static_assert(TIsDerivedFrom<PointedType, UObject>::IsDerived,
                              "FAny doesn't work with non-UObject derived pointers");
                Value = FInstancedStruct::Make<FUObjectStruct>(InValue);
            }
        }
        else Value = FInstancedStruct::Make(InValue);
    }

    // Type COPY_CONSTRUCTOR_GENERATE below here

    COPY_CONSTRUCTOR_GENERATE(bool, FBoolStruct)

    COPY_CONSTRUCTOR_GENERATE(uint8, FUint8Struct)

    COPY_CONSTRUCTOR_GENERATE(int32, FInt32Struct)

    COPY_CONSTRUCTOR_GENERATE(int64, FInt64Struct)

    COPY_CONSTRUCTOR_GENERATE(float, FFloatStruct)

    COPY_CONSTRUCTOR_GENERATE(FString, FFStringStruct)

    COPY_CONSTRUCTOR_GENERATE_PTR(const char, FFStringStruct)
    COPY_CONSTRUCTOR_GENERATE_PTR(char, FFStringStruct)
    COPY_CONSTRUCTOR_GENERATE_PTR(const WIDECHAR, FFStringStruct)
    COPY_CONSTRUCTOR_GENERATE_PTR(WIDECHAR, FFStringStruct)

    COPY_CONSTRUCTOR_GENERATE(FName, FFNameStruct)

    COPY_CONSTRUCTOR_GENERATE(FText, FFTextStruct)

    COPY_CONSTRUCTOR_GENERATE_PTR(UObject, FUObjectStruct)

    COPY_CONSTRUCTOR_GENERATE_TOBJECTPTR(UObject, FUObjectStruct)

    // Type COPY_CONSTRUCTOR_GENERATE above here

    template<typename T>
    FAny(T&& InValue)
    {
        if constexpr (TIsTObjectPtr<T>::Value)
        {
            using ObjectType = TRemoveObjectPointer<T>::Type;
            static_assert(TIsDerivedFrom<ObjectType, UObject>::IsDerived,
                          "FAny doesn't work with non-UObject derived TObjectPtrs");
            Value = FInstancedStruct::Make<FUObjectStruct>(MoveTemp(InValue));
        }
        else if constexpr (TIsPointer<T>::Value)
        {
            using PointedType = TRemovePointer<T>::Type;
            if constexpr (TIsPointer<PointedType>::Value)
            {
                using PointedPointedType = TRemovePointer<PointedType>::Type; // Can't think of better name
                static_assert(TIsDerivedFrom<PointedType, UObject>::IsDerived,
                              "FAny doesn't work with non-UObject derived pointers to pointers");
                Value = FInstancedStruct::Make<FUObjectStruct>(MoveTemp(*InValue));
            }
            else
            {
                static_assert(TIsDerivedFrom<PointedType, UObject>::IsDerived,
                              "FAny doesn't work with non-UObject derived pointers");
                Value = FInstancedStruct::Make<FUObjectStruct>(MoveTemp(InValue));
            }
        }
        else Value = FInstancedStruct::Make(MoveTemp(InValue));
    }

    // Type MOVE_CONSTRUCTOR_GENERATE below here

    MOVE_CONSTRUCTOR_GENERATE(bool, FBoolStruct)

    MOVE_CONSTRUCTOR_GENERATE(uint8, FUint8Struct)

    MOVE_CONSTRUCTOR_GENERATE(int32, FInt32Struct)

    MOVE_CONSTRUCTOR_GENERATE(int64, FInt64Struct)

    MOVE_CONSTRUCTOR_GENERATE(float, FFloatStruct)

    MOVE_CONSTRUCTOR_GENERATE(FString, FFStringStruct)

    MOVE_CONSTRUCTOR_GENERATE(FName, FFNameStruct)

    MOVE_CONSTRUCTOR_GENERATE(FText, FFTextStruct)

    MOVE_CONSTRUCTOR_GENERATE_PTR(UObject, FUObjectStruct)

    MOVE_CONSTRUCTOR_GENERATE_TOBJECTPTR(UObject, FUObjectStruct)

    // Type MOVE_CONSTRUCTOR_GENERATE above here

    template<typename Type, typename... Args>
    FAny(Args&&... Params) : Value(FInstancedStruct::Make<Type>(Forward<Args>(Params)...))
    {
    }

    inline FAny& operator=(const FAny&) = default;
    inline FAny& operator=(FAny&&) noexcept = default;

    inline FAny& operator=(const FInstancedStruct& InValue)
    {
        Value = InValue;
        return *this;
    }

    template<typename T>
    inline FAny& operator=(const T& InValue)
    {
        if constexpr (TIsTObjectPtr<T>::Value)
        {
            using ObjectType = TRemoveObjectPointer<T>::Type;
            static_assert(TIsDerivedFrom<ObjectType, UObject>::IsDerived,
                          "FAny doesn't work with non-UObject derived TObjectPtrs");
            Value = FInstancedStruct::Make<FUObjectStruct>(InValue);
        }
        else if constexpr (TIsPointer<T>::Value)
        {
            using PointedType = TRemovePointer<T>::Type;
            if constexpr (TIsPointer<PointedType>::Value)
            {
                using PointedPointedType = TRemovePointer<PointedType>::Type; // Can't think of better name
                static_assert(TIsDerivedFrom<PointedType, UObject>::IsDerived,
                              "FAny doesn't work with non-UObject derived pointers to pointers");
                Value = FInstancedStruct::Make<FUObjectStruct>(*InValue);
            }
            else
            {
                static_assert(TIsDerivedFrom<PointedType, UObject>::IsDerived,
                              "FAny doesn't work with non-UObject derived pointers");
                Value = FInstancedStruct::Make<FUObjectStruct>(InValue);
            }
        }
        else Value = FInstancedStruct::Make(InValue);
        return *this;
    }

    // Type COPY_ASSIGN_GENERATE below here

    COPY_ASSIGN_GENERATE(bool, FBoolStruct)

    COPY_ASSIGN_GENERATE(uint8, FUint8Struct)

    COPY_ASSIGN_GENERATE(int32, FInt32Struct)

    COPY_ASSIGN_GENERATE(int64, FInt64Struct)

    COPY_ASSIGN_GENERATE(float, FFloatStruct)

    COPY_ASSIGN_GENERATE(FString, FFStringStruct)

    COPY_ASSIGN_GENERATE_PTR(const char, FFStringStruct)
    COPY_ASSIGN_GENERATE_PTR(char, FFStringStruct)
    COPY_ASSIGN_GENERATE_PTR(const WIDECHAR, FFStringStruct)
    COPY_ASSIGN_GENERATE_PTR(WIDECHAR, FFStringStruct)

    COPY_ASSIGN_GENERATE(FName, FFNameStruct)

    COPY_ASSIGN_GENERATE(FText, FFTextStruct)

    COPY_ASSIGN_GENERATE_PTR(UObject, FUObjectStruct)

    COPY_ASSIGN_GENERATE_TOBJECTPTR(UObject, FUObjectStruct)

    // Type COPY_ASSIGN_GENERATE above here

    template<typename T>
    FAny& operator=(T&& InValue)
    {
        if constexpr (TIsTObjectPtr<T>::Value)
        {
            using ObjectType = TRemoveObjectPointer<T>::Type;
            static_assert(TIsDerivedFrom<ObjectType, UObject>::IsDerived,
                          "FAny doesn't work with non-UObject derived TObjectPtrs");
            Value = FInstancedStruct::Make<FUObjectStruct>(MoveTemp(InValue));
        }
        else if constexpr (TIsPointer<T>::Value)
        {
            using PointedType = TRemovePointer<T>::Type;
            if constexpr (TIsPointer<PointedType>::Value)
            {
                using PointedPointedType = TRemovePointer<PointedType>::Type; // Can't think of better name
                static_assert(TIsDerivedFrom<PointedType, UObject>::IsDerived,
                              "FAny doesn't work with non-UObject derived pointers to pointers");
                Value = FInstancedStruct::Make<FUObjectStruct>(MoveTemp(*InValue));
            }
            else
            {
                static_assert(TIsDerivedFrom<PointedType, UObject>::IsDerived,
                              "FAny doesn't work with non-UObject derived pointers");
                Value = FInstancedStruct::Make<FUObjectStruct>(MoveTemp(InValue));
            }
        }
        else Value = FInstancedStruct::Make(MoveTemp(InValue));
        return *this;
    }

    // Type MOVE_ASSIGN_GENERATE below here

    MOVE_ASSIGN_GENERATE(bool, FBoolStruct)

    MOVE_ASSIGN_GENERATE(uint8, FUint8Struct)

    MOVE_ASSIGN_GENERATE(int32, FInt32Struct)

    MOVE_ASSIGN_GENERATE(int64, FInt64Struct)

    MOVE_ASSIGN_GENERATE(float, FFloatStruct)

    MOVE_ASSIGN_GENERATE(FString, FFStringStruct)

    MOVE_ASSIGN_GENERATE(FName, FFNameStruct)

    MOVE_ASSIGN_GENERATE(FText, FFTextStruct)

    MOVE_ASSIGN_GENERATE_PTR(UObject, FUObjectStruct)

    MOVE_ASSIGN_GENERATE_TOBJECTPTR(UObject, FUObjectStruct)

    // Type MOVE_ASSIGN_GENERATE above here

    bool IsValid() const { return Value.IsValid(); }

    template<typename T>
    T* Get()
    {
        if constexpr (TIsDerivedFrom<T, UObject>::IsDerived)
        {
            if (Value.GetScriptStruct() != TBaseStructure<FUObjectStruct>::Get()) return nullptr;
            return Cast<T>(Value.GetMutable<FUObjectStruct>().Value.Get());
        }
        else return Value.GetScriptStruct() == TBaseStructure<T>::Get() ? Value.GetMutable<T>() : nullptr;
    }

    template<typename T>
    const T* Get() const
    {
        if constexpr (TIsDerivedFrom<T, UObject>::IsDerived)
        {
            if (Value.GetScriptStruct() != TBaseStructure<FUObjectStruct>::Get()) return nullptr;
            return Cast<T>(Value.Get<FUObjectStruct>().Value.Get());
        }
        else return Value.GetScriptStruct() == TBaseStructure<T>::Get() ? Value.Get<T>() : nullptr;
    }

    template<typename T>
    FAny& Set(const T& InValue)
    {
        if constexpr (TIsTObjectPtr<T>::Value)
        {
            using ObjectType = TRemoveObjectPointer<T>::Type;
            static_assert(TIsDerivedFrom<ObjectType, UObject>::IsDerived,
                          "FAny doesn't work with non-UObject derived TObjectPtrs");
            Value = FInstancedStruct::Make<FUObjectStruct>(InValue);
        }
        else if constexpr (TIsPointer<T>::Value)
        {
            using PointedType = TRemovePointer<T>::Type;
            if constexpr (TIsPointer<PointedType>::Value)
            {
                using PointedPointedType = TRemovePointer<PointedType>::Type; // Can't think of better name
                static_assert(TIsDerivedFrom<PointedType, UObject>::IsDerived,
                              "FAny doesn't work with non-UObject derived pointers to pointers");
                Value = FInstancedStruct::Make<FUObjectStruct>(*InValue);
            }
            else
            {
                static_assert(TIsDerivedFrom<PointedType, UObject>::IsDerived,
                              "FAny doesn't work with non-UObject derived pointers");
                Value = FInstancedStruct::Make<FUObjectStruct>(InValue);
            }
        }
        else Value = FInstancedStruct::Make(InValue);
        return *this;
    }

    template<typename T>
    FAny& Set(T&& InValue)
    {
        if constexpr (TIsTObjectPtr<T>::Value)
        {
            using ObjectType = TRemoveObjectPointer<T>::Type;
            static_assert(TIsDerivedFrom<ObjectType, UObject>::IsDerived,
                          "FAny doesn't work with non-UObject derived TObjectPtrs");
            Value = FInstancedStruct::Make<FUObjectStruct>(MoveTemp(InValue));
        }
        else if constexpr (TIsPointer<T>::Value)
        {
            using PointedType = TRemovePointer<T>::Type;
            if constexpr (TIsPointer<PointedType>::Value)
            {
                using PointedPointedType = TRemovePointer<PointedType>::Type; // Can't think of better name
                static_assert(TIsDerivedFrom<PointedType, UObject>::IsDerived,
                              "FAny doesn't work with non-UObject derived pointers to pointers");
                Value = FInstancedStruct::Make<FUObjectStruct>(MoveTemp(*InValue));
            }
            else
            {
                static_assert(TIsDerivedFrom<PointedType, UObject>::IsDerived,
                              "FAny doesn't work with non-UObject derived pointers");
                Value = FInstancedStruct::Make<FUObjectStruct>(MoveTemp(InValue));
            }
        }
        else Value = FInstancedStruct::Make(MoveTemp(InValue));
        return *this;
    }
};

// Type GET_GENERATE below here

GET_GENERATE(bool, FBoolStruct)

GET_GENERATE(uint8, FUint8Struct)

GET_GENERATE(int32, FInt32Struct)

GET_GENERATE(int64, FInt64Struct)

GET_GENERATE(float, FFloatStruct)

GET_GENERATE(FString, FFStringStruct)

GET_GENERATE(FName, FFNameStruct)

GET_GENERATE(FText, FFTextStruct)

template<>
inline UObject* FAny::Get<UObject>()
{
    if (FUObjectStruct* StructPtr = Value.GetMutablePtr<FUObjectStruct>())
    {
        return StructPtr->Value;
    }
    return nullptr;
}

template<>
inline const UObject* FAny::Get<UObject>() const
{
    if (const FUObjectStruct* StructPtr = Value.GetPtr<FUObjectStruct>())
    {
        return StructPtr->Value;
    }
    return nullptr;
}

GET_GENERATE_PTR(UObject, FUObjectStruct)

GET_GENERATE_TOBJECTPTR(UObject, FUObjectStruct)

// Type GET_GENERATE above here

// Type COPY_SET_GENERATE below here

COPY_SET_GENERATE(bool, FBoolStruct)

COPY_SET_GENERATE(uint8, FUint8Struct)

COPY_SET_GENERATE(int32, FInt32Struct)

COPY_SET_GENERATE(int64, FInt64Struct)

COPY_SET_GENERATE(float, FFloatStruct)

COPY_SET_GENERATE(FString, FFStringStruct)

COPY_SET_GENERATE_PTR(const char, FFStringStruct)
COPY_SET_GENERATE_PTR(char, FFStringStruct)
COPY_SET_GENERATE_PTR(const WIDECHAR, FFStringStruct)
COPY_SET_GENERATE_PTR(WIDECHAR, FFStringStruct)

COPY_SET_GENERATE(FName, FFNameStruct)

COPY_SET_GENERATE(FText, FFTextStruct)

COPY_SET_GENERATE_PTR(UObject, FUObjectStruct)

COPY_SET_GENERATE_TOBJECTPTR(UObject, FUObjectStruct)

// Type COPY_SET_GENERATE above here

// Type MOVE_SET_GENERATE below here

MOVE_SET_GENERATE(bool, FBoolStruct)

MOVE_SET_GENERATE(uint8, FUint8Struct)

MOVE_SET_GENERATE(int32, FInt32Struct)

MOVE_SET_GENERATE(int64, FInt64Struct)

MOVE_SET_GENERATE(float, FFloatStruct)

MOVE_SET_GENERATE(FString, FFStringStruct)

MOVE_SET_GENERATE_PTR(const char, FFStringStruct)
MOVE_SET_GENERATE_PTR(char, FFStringStruct)
MOVE_SET_GENERATE_PTR(const WIDECHAR, FFStringStruct)
MOVE_SET_GENERATE_PTR(WIDECHAR, FFStringStruct)

MOVE_SET_GENERATE(FName, FFNameStruct)

MOVE_SET_GENERATE(FText, FFTextStruct)

MOVE_SET_GENERATE_PTR(UObject, FUObjectStruct)

MOVE_SET_GENERATE_TOBJECTPTR(UObject, FUObjectStruct)

// Type MOVE_SET_GENERATE above here

#undef MOVE_SET_GENERATE_TOBJECTPTR
#undef MOVE_SET_GENERATE_PTR
#undef MOVE_SET_GENERATE
#undef COPY_SET_GENERATE_TOBJECTPTR
#undef COPY_SET_GENERATE_PTR
#undef COPY_SET_GENERATE
#undef GET_GENERATE_TOBJECTPTR
#undef GET_GENERATE_PTR
#undef GET_GENERATE
#undef MOVE_ASSIGN_GENERATE_TOBJECTPTR
#undef MOVE_ASSIGN_GENERATE_PTR
#undef MOVE_ASSIGN_GENERATE
#undef COPY_ASSIGN_GENERATE_TOBJECTPTR
#undef COPY_ASSIGN_GENERATE_PTR
#undef COPY_ASSIGN_GENERATE
#undef MOVE_CONSTRUCTOR_GENERATE_TOBJECTPTR
#undef MOVE_CONSTRUCTOR_GENERATE_PTR
#undef MOVE_CONSTRUCTOR_GENERATE
#undef COPY_CONSTRUCTOR_GENERATE_TOBJECTPTR
#undef COPY_CONSTRUCTOR_GENERATE_PTR
#undef COPY_CONSTRUCTOR_GENERATE
