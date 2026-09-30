#pragma once

#include "CoreMinimal.h"

// Unreal doesn't have its own built-in priority queue :(

template<typename InElementType, typename PredicateType = TGreater<InElementType>>
class TPriorityQueue
{
public:
    using ContainerType = TArray<InElementType>;
    using SizeType = ContainerType::SizeType;
    using ElementType = ContainerType::ElementType;
    using AllocatorType = ContainerType::AllocatorType;

    bool IsEmpty() const { return Container.Num() == 0; }
    int32 Num() const { return Container.Num(); }

    SizeType Push(const ElementType& Element) { return Container.HeapPush(Element, Predicate); }
    SizeType Push(ElementType&& Element) { return Container.HeapPush(Element, Predicate); }

    ElementType Pop(EAllowShrinking AllowShrinking = UE::Core::Private::AllowShrinkingByDefault<AllocatorType>())
    {
        ElementType Result;
        Container.HeapPop(Result, Predicate, AllowShrinking);
        return Result;
    }

    const ElementType& Top() const { return Container[0]; }
    ElementType& Top() { return Container[0]; }

    const ContainerType& GetContainer() const { return Container; }
    ContainerType& GetContainer() { return Container; }

private:
    ContainerType Container;
    PredicateType Predicate;
};
