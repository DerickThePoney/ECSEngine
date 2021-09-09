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

private:
    std::vector<EntityId> FBuildingInNeedForStorage;
};
} // namespace ECSEngine