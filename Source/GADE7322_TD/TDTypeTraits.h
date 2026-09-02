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
