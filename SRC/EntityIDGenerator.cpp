#include "stdafx.h"

#include "EntityIDGenerator.h"

#include "EntityId.h"

namespace ECSEngine
{

EntityIDGenerator::EntityIDGenerator(u32 parWorldID)
    : FAssociatedWorldID(parWorldID)
    , FNextIncrementalId(0)
{
}

EntityIDGenerator::EntityIDGenerator(EntityIDGenerator&& other)
{
    FAssociatedWorldID = other.FAssociatedWorldID;
    FNextIncrementalId = other.FNextIncrementalId;
    FReusableIds = std::move(other.FReusableIds);

    other.FAssociatedWorldID = -1;
    other.FNextIncrementalId = -1;
}

void EntityIDGenerator::operator=(EntityIDGenerator&& other) noexcept
{
    FAssociatedWorldID = other.FAssociatedWorldID;
    FNextIncrementalId = other.FNextIncrementalId;
    FReusableIds = std::move(other.FReusableIds);

    other.FAssociatedWorldID = -1;
    other.FNextIncrementalId = -1;
}

EntityIDGenerator::~EntityIDGenerator()
{
}

EntityId EntityIDGenerator::GetNextEntityId()
{
    AssertRelease(FAssociatedWorldID != -1);
    if (FReusableIds.empty())
        return EntityId(FAssociatedWorldID, FNextIncrementalId++);

    EntityId id(FAssociatedWorldID, FReusableIds.front());
    FReusableIds.pop();
    return id;
}

void EntityIDGenerator::ReleaseEntityId(const EntityId& parId)
{
    AssertRelease(FAssociatedWorldID != -1);
    AlwaysCheckedAssert(parId.GetWorldId() == FAssociatedWorldID);
    FReusableIds.push(parId.GetSequentialId());
}

} // namespace ECSEngine