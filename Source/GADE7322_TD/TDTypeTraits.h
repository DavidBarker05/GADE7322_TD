#pragma once

#include "CoreMinimal.h"

// This exists because TIsPointer<T>::Value uses an enum which doesn't
// work with C++ requires, so I added TIsPointer_V and added some other nicer
// type traits too

template<typename T>
constexpr bool TIsPointer_V = false;

template<typename T>
constexpr bool TIsPointer_V<T*> = true;

template<typename T>
constexpr bool TIsPointer_V<const T> = TIsPointer_V<T>;

template<typename T>
constexpr bool TIsPointer_V<volatile T> = TIsPointer_V<T>;

template<typename T>
constexpr bool TIsPointer_V<const volatile T> = TIsPointer_V<T>;

template<typename, typename>
constexpr bool TIsSame_V = false;

template<typename T>
constexpr bool TIsSame_V<T, T> = true;

template<typename T1, typename T2>
constexpr bool TIsDerivedFrom_V = TIsDerivedFrom<T1, T2>::Value;

template<typename T>
using TDecay_T = typename TDecay<T>::Type;

template<typename T>
using TRemoveObjectPointer_T = typename TRemoveObjectPointer<T>::Type;

template<typename T>
using TRemovePointer_T = typename TRemovePointer<T>::Type;

template<typename T>
struct TRemoveCV
{
    using type = T;
    using Type = T;
};

template<typename T>
struct TRemoveCV<const T>
{
    using type = T;
    using Type = T;
};

template<typename T>
struct TRemoveCV<volatile T>
{
    using type = T;
    using Type = T;
};

template<typename T>
struct TRemoveCV<const volatile T>
{
    using type = T;
    using Type = T;
};

template<typename T>
using TRemoveCV_T = typename TRemoveCV<T>::type;

template<bool Predicate, typename Result = void>
using TEnableIf_T = typename TEnableIf<Predicate, Result>::Type;

template<typename T>
constexpr bool TIsFloatingPoint_V = false;

template<>
constexpr bool TIsFloatingPoint_V<float> = true;

template<>
constexpr bool TIsFloatingPoint_V<const float> = true;

template<>
constexpr bool TIsFloatingPoint_V<volatile float> = true;

template<>
constexpr bool TIsFloatingPoint_V<const volatile float> = true;

template<>
constexpr bool TIsFloatingPoint_V<double> = true;

template<>
constexpr bool TIsFloatingPoint_V<const double> = true;

template<>
constexpr bool TIsFloatingPoint_V<volatile double> = true;

template<>
constexpr bool TIsFloatingPoint_V<const volatile double> = true;

template<>
constexpr bool TIsFloatingPoint_V<long double> = true;

template<>
constexpr bool TIsFloatingPoint_V<const long double> = true;

template<>
constexpr bool TIsFloatingPoint_V<volatile long double> = true;

template<>
constexpr bool TIsFloatingPoint_V<const volatile long double> = true;
