#pragma once
#include "Common/Singleton.h"
#include "EntityWorld.h"
#include "WorldIds.h"
namespace ECSEngine
{

class WorldManager final : public Singleton<WorldManager>
{
public:
    WorldManager();
    virtual ~WorldManager();

    void Init();
    void Destroy();

    EntityWorld& GetWorld(Worlds::Type parWorld);
    EntityWorld* GetWorldIFP(Worlds::Type parWorld);

private:
    std::unordered_map<Worlds::Type, EntityWorld> FWorlds;
};
} // namespace ECSEngine