#include "stdafx.h"

#include "WorldManager.h"
#include "ECSGameplay_Common/WorldIds.h"

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
    FWorlds.resize((u32)EEntityWorlds::LENGTH);
}

void WorldManager::Shutdown()
{
    FWorlds.clear();
}

EntityWorld& WorldManager::GetWorld(EEntityWorlds parWorld)
{
    return *FWorlds[(u32)parWorld];
}

EntityWorld* WorldManager::GetWorldIFP(EEntityWorlds parWorld)
{
    return FWorlds[(u32)parWorld].get();
}

void WorldManager::AddEntityWorldStealOwnership(EEntityWorlds parType, EntityWorld* parWorld)
{
    AssertRelease(parType < EEntityWorlds::LENGTH);
    AssertRelease(parWorld != nullptr);
    AssertRelease(FWorlds[(u32)parType] == nullptr);

    FWorlds[(u32)parType] = std::unique_ptr<EntityWorld>(parWorld);
}

void WorldManager::DestroyAllRemainingEntities()
{
    foreachitem(world, FWorlds) { world->DestroyAllRemainingEntities(); }
}

void WorldManager::MarkAsDead(const EntityId& parId)
{
    AssertRelease(parId.Valid());
#ifdef PERFORM_SECURITY_CHECKS
    EntityWorld* world = WorldManager::Instance().GetWorldIFP((EEntityWorlds)parId.GetWorldId());
    AssertRelease(world != nullptr);
#endif
    FDeadEntities.insert(parId);
}

void WorldManager::ProcessDestroyEntities()
{
    foreachitemconst(id, FDeadEntities)
    {
        EntityWorld* world = WorldManager::Instance().GetWorldIFP((EEntityWorlds)id.GetWorldId());
        AssertRelease(world != nullptr);

        world->DestroyEntity(id);
    }
    FDeadEntities.clear();
}

} // namespace ECSEngine
