#pragma once
#include "Common/Singleton.h"
#include "EntityWorld.h"
#include "WorldIds.h"
namespace ECSEngine
{
class EntityWorld;
class WorldManager final : public Singleton<WorldManager>
{
public:
    WorldManager();
    virtual ~WorldManager();

    void Init();
    void Shutdown();

    void AddEntityWorldStealOwnership(EEntityWorlds parType, EntityWorld* parWorld);
    void ProcessDestroyEntities();
    void DestroyAllRemainingEntities();

    EntityWorld& GetWorld(EEntityWorlds parWorld);
    EntityWorld* GetWorldIFP(EEntityWorlds parWorld);

    void MarkAsDead(const EntityId& parId);

private:
    std::vector<std::unique_ptr<EntityWorld>> FWorlds;

    std::set<EntityId> FDeadEntities;
};
} // namespace ECSEngine
