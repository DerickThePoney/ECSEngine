#pragma once
#include "Common/Singleton.h"
#include "ECSCore/EntityId.h"
#include "ECSCore/ModuleSystem.h"
#include "GameResources.h"

namespace ECSEngine
{
class StorageSlotModule;
using ResourceToStoragePair = std::pair<u32, std::set<EntityId>>;
class ResourceManager final : public ModuleSystem, public Singleton<ResourceManager>
{
public:
    ResourceManager();
    ~ResourceManager();

    // Hackos
    void Finalize();
    static void Delete();

    u32 GetResourceQuantity(GameResource::Type parResource) const;
    const ResourceToStoragePair* GetStoragesForResourceIFP(GameResource::Type parResource) const;
    void ConsumeFromStorage(GameResource::Type parResource, u32 parQuantity, StorageSlotModule* parStorageSlotModule);

protected:
    void VirtualUpdate() override;

private:
    std::unordered_map<GameResource::Type, ResourceToStoragePair> FResources;
};
} // namespace ECSEngine
