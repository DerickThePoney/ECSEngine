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

} // namespace ECSEngine