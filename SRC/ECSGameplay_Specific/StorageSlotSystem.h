#pragma once
#include "ECSCore/ModuleSystem.h"

namespace ECSEngine
{
template<typename Module>
class ModuleAccessor;
class StorageSlotModule;
class PositionModule;
class LinkToStorageModule;
class RawResourceProductionModule;
class ResourceStorageModule;

struct BuildingNeedsStorageMessage;
class StorageSlotSystem final : public ModuleSystem
{
public:
    StorageSlotSystem();
    ~StorageSlotSystem() = default;
    void ProcessMessages(const BuildingNeedsStorageMessage& parMessage,
          ModuleAccessor<StorageSlotModule>& parStorageSlotAccessor,
          ModuleAccessor<PositionModule>& parPositionModuleAccessor,
          ModuleAccessor<LinkToStorageModule>& parLinkToStorageAccessor,
          ModuleAccessor<RawResourceProductionModule>& parRawProductionAccessor);

protected:
    void VirtualUpdate() override;

    void TransfertResourcesFromRawProducersToStorage(ModuleAccessor<ResourceStorageModule>& parResourceStorageAccessor,
          ModuleAccessor<StorageSlotModule>& parStorageSlotAccessor,
          ModuleAccessor<LinkToStorageModule>& parLinkToStorageAccessor,
          ModuleAccessor<RawResourceProductionModule>& parRawProductionAccessor);

private:
    std::vector<EntityId> FBuildingInNeedForStorage;
};
} // namespace ECSEngine