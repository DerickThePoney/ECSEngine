#include "stdafx.h"

#include "WorldManager.h"

#include "Common/SavingSystemImplementation.h"
#include "EntityWorld.h"
#include "WorldIds.h"

namespace ECSEngine
{

IMPLEMENT_SAVELOAD_ABILITIES(WorldManager);
template<typename Chunk, bool isWriting>
void WorldManager::SaveLoad(Chunk& parChunk)
{
    if (isWriting)
    {
        AlwaysCheckedAssertMsg(FDeadEntities.empty(), "Please do not save before have killed the entities, thank you !");
        u32 nonNullWorlds = 0;
        foreachitem(world, FWorlds)
        {
            if (world != nullptr)
                nonNullWorlds++;
        }
        parChunk& nonNullWorlds;

        foreachitem(world, FWorlds)
        {
            if (world == nullptr)
                continue;

            u32 worldId = world->WorldID();
            parChunk& worldId;
            EntityWorld& worldRef = *(world.get());
            parChunk& worldRef;
        }
    }
    else
    {
        u32 nonNullWorlds = 0;
        parChunk& nonNullWorlds;

        forrange(i, 0, nonNullWorlds)
        {
            u32 worldId;
            parChunk& worldId;

            AssertRelease(FWorlds[worldId] != nullptr);
            EntityWorld& worldRef = *(FWorlds[worldId]);
            parChunk& worldRef;
        }
    }
}

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
    foreachitem(world, FWorlds)
    {
        if (world != nullptr)
            world->DestroyAllRemainingEntities();
    }
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

void WorldManager::RegisterDeathListener(UnitDeathListener parListener)
{
#ifdef ENABLE_SECURITY_CHECKS
    foreachitemconst(listener, FUnitDeathListeners) AlwaysCheckedAssert(listener != parListener);
#endif
    FUnitDeathListeners.push_back(parListener);
}

void WorldManager::RemoveDeathListener(UnitDeathListener parListener)
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
