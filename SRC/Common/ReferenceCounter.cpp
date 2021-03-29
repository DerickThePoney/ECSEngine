#include "stdafx.h"

#include "ReferenceCounter.h"

namespace ECSEngine
{
IMPLEMENT_POOL_ALLOCATED_CUSTOM_CHUNK_SIZE(ReferenceCounter, ReferenceCounterPoolSize);
ReferenceCounter::ReferenceCounter()
{
}

ReferenceCounter::~ReferenceCounter()
{
}

void ReferenceCounter::IncrementRefCounter()
{
    FRefCounts += 1;
}

i32 ReferenceCounter::DecrementRefCounter()
{
    FRefCounts -= 1;
    return FRefCounts;
}

i32 ReferenceCounter::GetRefCounts() const
{
    return FRefCounts;
}
} // namespace ECSEngine
