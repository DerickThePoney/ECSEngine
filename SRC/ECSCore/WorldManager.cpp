#include "stdafx.h"

#include "WorldManager.h"

#include "EntityWorld.h"
#include "WorldIds.h"

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

const EntityWorld& WorldManager::GetWorld(EEntityWorlds parWorld) const
{
    return *FWorlds[(u32)parWorld];
}

EntityWorld* WorldManager::GetWorldIFP(EEntityWorlds parWorld)
{
    return FWorlds[(u32)parWorld].get();
}

const EntityWorld* WorldManager::GetWorldIFP(EEntityWorlds parWorld) const
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
#ifdef ENABLE_SECURITY_CHECKS
    EntityWorld* world = WorldManager::Instance().GetWorldIFP((EEntityWorlds)parId.GetWorldId());
    AssertRelease(world != nullptr);
#endif
    FDeadEntities.insert(parId);
}

const EntityTemplate* WorldManager::GetTemplateForEntityId(const EntityId& parUnitId) const
{
    AssertRelease(parUnitId.Valid());
    const EntityWorld& world = GetWorld(parUnitId.GetWorld());
    return world.GetTemplateForEntity(parUnitId);
}

void WorldManager::RegisterListener(UnitDeathListener parListener)
{
#ifdef ENABLE_SECURITY_CHECKS
    foreachitemconst(listener, FUnitDeathListeners) AlwaysCheckedAssert(listener != parListener);
#endif
    FUnitDeathListeners.push_back(parListener);
}

void WorldManager::RemoveListener(UnitDeathListener parListener)
{
    auto pos = FUnitDeathListeners.end();
    for (auto it = FUnitDeathListeners.begin(); it != FUnitDeathListeners.end(); ++it)
    {
        if (*it == parListener)
        {
            pos = it;
            break;
        }
    }
    AlwaysCheckedAssert(pos != FUnitDeathListeners.end());
    FUnitDeathListeners.erase(pos);
#ifdef ENABLE_SECURITY_CHECKS
    foreachitemconst(listener, FUnitDeathListeners) AlwaysCheckedAssert(listener != parListener);
#endif
}

void WorldManager::ProcessDestroyEntities()
{
    foreachitemconst(id, FDeadEntities)
    {
        foreachitemconst(listener, FUnitDeathListeners) listener(id);

        EntityWorld* world = WorldManager::Instance().GetWorldIFP((EEntityWorlds)id.GetWorldId());
        AssertRelease(world != nullptr);

        world->DestroyEntity(id);
    }
    FDeadEntities.clear();
}

} // namespace ECSEngine
