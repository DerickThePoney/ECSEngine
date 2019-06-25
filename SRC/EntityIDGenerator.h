#pragma once

namespace ECSEngine
{
class EntityId;
}

namespace ECSEngine
{
class EntityIDGenerator
{
public:
    EntityIDGenerator(u32 parWorldID = -1);
    ~EntityIDGenerator();
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
    u32 FNextIncrementalId;
    std::queue<u32> FReusableIds;
};
} // namespace ECSEngine
