#include "stdafx.h"

#include "EntityIDGenerator.h"

#include "EntityId.h"
#include "WorldIds.h"

namespace ECSEngine
{

EntityIDGenerator::EntityIDGenerator(u32 parWorldID)
    : IdGenerator()
    , FAssociatedWorldID(EEntityWorlds::LENGTH)
{
}

EntityIDGenerator::EntityIDGenerator(EntityIDGenerator&& other)
{
    FAssociatedWorldID = other.FAssociatedWorldID;

    other.FAssociatedWorldID = EEntityWorlds::LENGTH;
}

void EntityIDGenerator::operator=(EntityIDGenerator&& other) noexcept
{
    FAssociatedWorldID = other.FAssociatedWorldID;
    other.FAssociatedWorldID = EEntityWorlds::LENGTH;
}

EntityIDGenerator::~EntityIDGenerator()
{
}

EntityId EntityIDGenerator::GetNextEntityId()
{
    AssertRelease(FAssociatedWorldID != EEntityWorlds::LENGTH);
    EntityId nextId = EntityId((u32)FAssociatedWorldID, GetNextId());
    AssertRelease(nextId.Valid());
    AssertRelease(nextId.GetWorld() == FAssociatedWorldID);
    return nextId;
}

void EntityIDGenerator::ReleaseEntityId(const EntityId& parId)
{
    AssertRelease(FAssociatedWorldID != EEntityWorlds::LENGTH);
    AlwaysCheckedAssert(parId.GetWorld() == FAssociatedWorldID);
    ReleaseId(parId.GetSequentialId());
}

} // namespace ECSEngine
