#include "stdafx.h"

#include "ResourceManager.h"

#include "ECSCore/ModuleAccessor.h"
#include "StorageSlotModule.h"

namespace ECSEngine
{
ResourceManager::ResourceManager()
    : Singleton()
    , ModuleSystem()
{
    RegisterDepency<StorageSlotModule>(Worlds::BUILDINGS);
}

ResourceManager::~ResourceManager()
{
}

void ResourceManager::VirtualUpdate()
{
    ModuleSystem::VirtualUpdate();

    ModuleAccessor<StorageSlotModule> storageSlotAccessor(Worlds::BUILDINGS);

    FResources.clear();
    foreachitemconst(storageSlotModule, storageSlotAccessor)
    {
        MemoryView<const StorageSlot> slots = storageSlotModule.StorageSlots();
        foreachitemconst(slot, slots)
        {
            if (!slot.FReservedForBuilding.Valid())
                continue;

            auto itFind = FResources.find(slot.Resource);
            if (itFind == FResources.end())
            {
                ResourceToStoragePair pair;
                pair.first = slot.Quantity;
                pair.second.insert(slot.FReservedForBuilding);
                FResources.insert_or_assign(slot.Resource, pair);
            }
            else
            {
                itFind->second.first += slot.Quantity;
                itFind->second.second.insert(slot.FReservedForBuilding);
            }
        }
    }
}

void ResourceManager::Finalize()
{
    ModuleSystem::Destroy();
}

void ResourceManager::Delete()
{
    Singleton<ResourceManager>::Destroy();
}

} // namespace ECSEngine