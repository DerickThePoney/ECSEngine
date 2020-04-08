#pragma once
#include "PoolAllocator.h"

namespace ECSEngine
{
#ifdef PERFORM_SECURITY_CHECKS
class ReferenceCounter
{
    DECLARE_POOL_ALLOCATED_CUSTOM_CHUNK_SIZE(ReferenceCounter, 8192);

public:
    ReferenceCounter();
    ~ReferenceCounter();

    ReferenceCounter(const ReferenceCounter& other) = delete;
    ReferenceCounter(ReferenceCounter&& other) = delete;

    void operator=(const ReferenceCounter& other) = delete;
    void operator=(ReferenceCounter&& other) = delete;

    void IncrementRefCounter();
    i32 DecrementRefCounter();
    i32 GetRefCounts() const;

private:
    i32 FRefCounts = 0;
};
#endif
} // namespace ECSEngine