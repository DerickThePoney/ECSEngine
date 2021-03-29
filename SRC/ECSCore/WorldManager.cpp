#include "stdafx.h"

#include "WorldManager.h"

namespace ECSEngine
{

WorldManager::WorldManager()
    : Singleton<WorldManager>()
{
}

WorldManager::~WorldManager()
{
    Shutdown();
}

void WorldManager::Init()
{
    FWorlds.resize(Worlds::LENGTH);
}

void WorldManager::Shutdown()
{
    FWorlds.clear();
}

EntityWorld& WorldManager::GetWorld(Worlds::Type parWorld)
{
    return *FWorlds[parWorld];
}

EntityWorld* WorldManager::GetWorldIFP(Worlds::Type parWorld)
{
    return FWorlds[parWorld].get();
}

void WorldManager::AddEntityWorldStealOwnership(Worlds::Type parType, EntityWorld* parWorld)
{
    AssertRelease(parType < Worlds::LENGTH);
    AssertRelease(parWorld != nullptr);
    AssertRelease(FWorlds[parType] == nullptr);

    FWorlds[parType] = std::unique_ptr<EntityWorld>(parWorld);
}

void WorldManager::DestroyAllRemainingEntities()
{
    foreachitem(world, FWorlds) { world->DestroyAllRemainingEntities(); }
}

void WorldManager::MarkAsDead(const EntityId& parId)
{
    AssertRelease(parId.Valid());
#ifdef PERFORM_SECURITY_CHECKS
    EntityWorld* world = WorldManager::Instance().GetWorldIFP((Worlds::Type)parId.GetWorldId());
    AssertRelease(world != nullptr);
#endif
    FDeadEntities.insert(parId);
}

void WorldManager::ProcessDestroyEntities()
{
    foreachitemconst(id, FDeadEntities)
    {
        EntityWorld* world = WorldManager::Instance().GetWorldIFP((Worlds::Type)id.GetWorldId());
        AssertRelease(world != nullptr);

        world->DestroyEntity(id);
    }
    FDeadEntities.clear();
}

} // namespace ECSEngine
