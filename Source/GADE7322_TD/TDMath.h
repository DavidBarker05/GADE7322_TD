#pragma once

#include "CoreMinimal.h"

#include "TDTypeTraits.h"

// FMath doesn't contain an inverse lerp :/ so here we are

struct FTDMath : FMath
{
    template<typename T>
    struct TCustomInverseLerp
    {
        constexpr static bool Value = false;
    };

    template<typename T>
    constexpr static bool TCustomInverseLerp_V = TCustomInverseLerp<T>::Value;

    template<typename T, typename U>
    requires(!TCustomInverseLerp_V<T> && (TIsFloatingPoint_V<U> || TIsSame_V<T, U>) && !TIsSame_V<T, bool>)
    [[nodiscard]] static constexpr UE_FORCEINLINE_HINT T InverseLerp(const T& A, const U& B, const U& Value)
    {
        return A != B ? static_cast<T>((Value - A) / (B - A)) : T {};
    }

    template<typename T, typename U>
    requires TCustomInverseLerp_V<T>
    [[nodiscard]] static UE_FORCEINLINE_HINT T InverseLerp(const T& A, const T& B, const U& Value)
    {
        return TCustomInverseLerp<T>::InverseLerp(A, B, Value);
    }

    template<typename T1, typename T2, typename T3>
    requires(!TIsSame_V<T1, T2> && !TCustomInverseLerp_V<T1> && !TCustomInverseLerp_V<T2>)
    [[nodiscard]] static auto InverseLerp(const T1& A, const T2& B, const T3& Value) -> decltype(A * B)
    {
        using ABType = decltype(A * B);
        return InverseLerp(ABType(A), ABType(B), Value);
    }
};
