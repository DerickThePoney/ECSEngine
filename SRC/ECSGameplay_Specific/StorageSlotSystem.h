#pragma once
#include "ECSCore/ModuleSystem.h"

namespace ECSEngine
{
struct BuildingNeedsStorageMessage;
class StorageSlotSystem final : public ModuleSystem
{
public:
    StorageSlotSystem();
    ~StorageSlotSystem() = default;
    void ProcessMessages(const BuildingNeedsStorageMessage& parMessage);

protected:
    void VirtualUpdate() override;

private:
    std::vector<EntityId> FBuildingInNeedForStorage;
};
} // namespace ECSEngine