#include "stdafx.h"

#include "EntityIDGenerator.h"

#include "EntityId.h"

namespace ECSEngine
{

EntityIDGenerator::EntityIDGenerator(u32 parWorldID)
    : IdGenerator()
    , FAssociatedWorldID(parWorldID)
{
}

EntityIDGenerator::EntityIDGenerator(EntityIDGenerator&& other)
{
    FAssociatedWorldID = other.FAssociatedWorldID;

    other.FAssociatedWorldID = -1;
}

void EntityIDGenerator::operator=(EntityIDGenerator&& other) noexcept
{
    FAssociatedWorldID = other.FAssociatedWorldID;
    other.FAssociatedWorldID = -1;
}

EntityIDGenerator::~EntityIDGenerator()
{
}

EntityId EntityIDGenerator::GetNextEntityId()
{
    AssertRelease(FAssociatedWorldID != -1);
    return EntityId(FAssociatedWorldID, GetNextId());
}

void EntityIDGenerator::ReleaseEntityId(const EntityId& parId)
{
    AssertRelease(FAssociatedWorldID != -1);
    AlwaysCheckedAssert(parId.GetWorldId() == FAssociatedWorldID);
    ReleaseId(parId.GetSequentialId());
}

} // namespace ECSEngine