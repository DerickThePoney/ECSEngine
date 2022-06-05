#pragma once
#include "ECSCore/ModuleSystem.h"

namespace ECSEngine
{
template<typename Module>
class ModuleAccessor;
class StorageSlotModule;
class PositionModule;
class LinkToStorageModule;
class RecipeProductionModule;
class ResourceStorageModule;
class EntityId;

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
          ModuleAccessor<RecipeProductionModule>& parRecipeProductionAccessor);

protected:
    void VirtualUpdate() override;

    void TransfertResourcesFromProducersToStorage(ModuleAccessor<ResourceStorageModule>& parResourceStorageAccessor,
          ModuleAccessor<StorageSlotModule>& parStorageSlotAccessor,
          ModuleAccessor<LinkToStorageModule>& parLinkToStorageAccessor,
          ModuleAccessor<RecipeProductionModule>& parRecipeProductionAccessor);

    void TransfertResourcesFromStoragesToProducers(ModuleAccessor<ResourceStorageModule>& parResourceStorageAccessor,
          ModuleAccessor<StorageSlotModule>& parStorageSlotAccessor,
          ModuleAccessor<LinkToStorageModule>& parLinkToStorageAccessor,
          ModuleAccessor<RecipeProductionModule>& parRecipeProductionAccessor);

private:
    std::vector<EntityId> FBuildingInNeedForStorage;
};
} // namespace ECSEngine