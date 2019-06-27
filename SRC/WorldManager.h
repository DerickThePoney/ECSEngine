#pragma once
#include "EntityWorld.h"
#include "Singleton.h"
namespace ECSEngine
{
namespace Worlds
{
enum Type
{
    STANDARD,
    LENGTH
};
}

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