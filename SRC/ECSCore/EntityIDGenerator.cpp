#include "stdafx.h"

#include "EntityIDGenerator.h"

#include "Common/SavingSystemImplementation.h"
#include "EntityId.h"
#include "WorldIds.h"

namespace ECSEngine
{

IMPLEMENT_SAVELOAD_ABILITIES(EntityIDGenerator);
template<typename Chunk, bool isWriting>
void EntityIDGenerator::SaveLoad(Chunk& parChunk)
{
    IdGenerator::SaveLoad(parChunk);

    u32 worldId = (u32)FAssociatedWorldID;
    parChunk& worldId;
    if (!isWriting)
        FAssociatedWorldID = (EEntityWorlds)worldId;
}

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
