#pragma once
#include "Common/PoolAllocator.h"
#include "ECSCore/EntityId.h"

namespace ECSEngine
{

struct BuildingNeedsStorageMessage
{
    static constexpr u32 PoolSize = 32;
    DECLARE_POOL_ALLOCATED_CUSTOM_CHUNK_SIZE(BuildingNeedsStorageMessage, PoolSize);

public:
    EntityId FUnitId;
};
} // namespace ECSEngine
