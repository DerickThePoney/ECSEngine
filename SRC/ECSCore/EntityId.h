#pragma once
#include "Common/RefCountedObject.h"
#include "WorldIds.h"
namespace ECSEngine
{
#pragma pack(push, 1)
struct PackedEntityId
{
    PackedEntityId(u32 parWorldId, u32 parId)
        : FId((parWorldId & 0xFF) << 24 | parId)
    {
    }

    const u8 GetWorldId() const { return FId >> 24 & 0xFF; }
    const u32 GetSequentialId() const { return FId & 0xFFFFFF; }

    bool operator==(const PackedEntityId& other) const { return FId == other.FId; }

private:
    u32 FId;
};

class EntityId
{
public:
    explicit EntityId(u32 parWorldId = 0xFF, u32 parId = 0xFFFFFF);

    const u8 GetWorldId() const { return FId.GetWorldId(); }
    const Worlds::Type GetWorld() const { return (Worlds::Type)GetWorldId(); }
    const u32 GetSequentialId() const { return FId.GetSequentialId(); }
    bool Valid() const { return FId.GetWorldId() != 0xFF && FId.GetSequentialId() != 0xFFFFFF; }

    bool operator==(const EntityId& other) const { return FId == other.FId; }
    bool operator!=(const EntityId& other) const { return !(FId == other.FId); }

private:
    PackedEntityId FId;
};
#pragma pack(pop)

static bool operator<(const EntityId& left, const EntityId& right)
{
    return left.GetWorldId() < right.GetWorldId() || left.GetSequentialId() < right.GetSequentialId();
}

} // namespace ECSEngine
