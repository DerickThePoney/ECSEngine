#pragma once
#include "Common/PoolAllocator.h"

namespace ECSEngine
{

struct ConstructBuildingMessage
{
    static constexpr u32 PoolSize = 32;
    DECLARE_POOL_ALLOCATED_CUSTOM_CHUNK_SIZE(ConstructBuildingMessage, PoolSize);

public:
    std::string FTemplateName;
    glm::vec3 FPosition;
};
} // namespace ECSEngine
