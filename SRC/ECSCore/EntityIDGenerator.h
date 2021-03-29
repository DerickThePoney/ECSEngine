#pragma once
#include "Common/IdGenerator.h"

namespace ECSEngine
{
class EntityId;
}

namespace ECSEngine
{
class EntityIDGenerator : public IdGenerator
{
public:
    EntityIDGenerator(u32 parWorldID = 0xFF);
    virtual ~EntityIDGenerator();
    EntityIDGenerator(EntityIDGenerator&& other);

    void operator=(EntityIDGenerator&& other) noexcept;

    EntityIDGenerator(const EntityIDGenerator& other) = delete;
    void operator=(const EntityIDGenerator& other) = delete;

    EntityId GetNextEntityId();
    void ReleaseEntityId(const EntityId& parId);

    void SetWorldId(const u32 parWorldId) { FAssociatedWorldID = parWorldId; }
    const u32 GetWorldId() const { return FAssociatedWorldID; }

private:
    u32 FAssociatedWorldID;
};
} // namespace ECSEngine
