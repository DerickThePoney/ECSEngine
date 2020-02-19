#include "stdafx.h"

#include "WorldManager.h"

#include "ECSGameplay_Base/WorldDeclaration.h"

namespace ECSEngine
{

WorldManager::WorldManager()
    : Singleton<WorldManager>()
{
}

WorldManager::~WorldManager()
{
}

void WorldManager::Init()
{
    FWorlds.insert_or_assign(Worlds::STANDARD, EntityWorld());
    CreateWorld(FWorlds.at(Worlds::STANDARD));
}

void WorldManager::Destroy()
{
    FWorlds.clear();
}

ECSEngine::EntityWorld& WorldManager::GetWorld(Worlds::Type parWorld)
{
    auto it = FWorlds.find(parWorld);
    AssertRelease(it != FWorlds.end());
    return FWorlds.at(parWorld);
}

ECSEngine::EntityWorld* WorldManager::GetWorldIFP(Worlds::Type parWorld)
{
    auto it = FWorlds.find(parWorld);
    if (it == FWorlds.end())
        return nullptr;
    return &it->second;
}

} // namespace ECSEngine