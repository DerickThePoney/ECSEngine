#include "stdafx.h"

#include "StackAllocator.h"

namespace ECSEngine
{

StackAllocator::StackAllocator(u32 parCapacity)
    : FCapacity(parCapacity)
    , FAllocated(0)
{
    FData = reinterpret_cast<uc8*>(malloc(parCapacity));
}

StackAllocator::~StackAllocator()
{
    AssertRelease(FData != nullptr);
    free(FData);
}

void* StackAllocator::Allocate(const u32 parSize)
{
    AssertRelease(parSize + FAllocated < FCapacity);
    const u32 currentlyAllocated = FAllocated;
    FAllocated += parSize;
    return FData + currentlyAllocated;
}

void StackAllocator::Free(const u32 parSize)
{
    AssertRelease(FAllocated > parSize);
    FAllocated -= parSize;
}

} // namespace ECSEngine