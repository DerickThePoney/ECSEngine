#pragma once
#include "Common/Singleton.h"
#include "ECSCore/EntityId.h"
#include "GameResources.h"

namespace ECSEngine
{
using ResourceToStoragePair = std::pair<u32, std::set<EntityId>>;
class ResourceManager : public Singleton<ResourceManager>
{
public:
    ResourceManager();
    ~ResourceManager();

private:
    std::unordered_map<GameResource::Type, ResourceToStoragePair> FResources;
};
} // namespace ECSEngine
