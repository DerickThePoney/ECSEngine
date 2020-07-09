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

    void AddEntityWorldStealOwnership(Worlds::Type parType, EntityWorld* parWorld);
    void DestroyAllRemainingEntities();

    EntityWorld& GetWorld(Worlds::Type parWorld);
    EntityWorld* GetWorldIFP(Worlds::Type parWorld);

private:
    std::vector<std::unique_ptr<EntityWorld>> FWorlds;
};
} // namespace ECSEngine