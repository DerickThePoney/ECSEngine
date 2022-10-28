#include "stdafx.h"

#include "ResourceManager.h"

#include "ECSCore/ModuleAccessor.h"
#include "ECSCore/WorldIds.h"
#include "StorageSlotModule.h"

namespace ECSEngine
{
ResourceManager::ResourceManager()
    : Singleton()
    , ModuleSystem()
{
    RegisterDepency<StorageSlotModule>(EEntityWorlds::BUILDINGS);
}

ResourceManager::~ResourceManager()
{
}

void ResourceManager::VirtualUpdate()
{
    ModuleSystem::VirtualUpdate();

    ModuleAccessor<StorageSlotModule> storageSlotAccessor(EEntityWorlds::BUILDINGS);

    FResources.clear();
    foreachitemconst(storageSlotModule, storageSlotAccessor)
    {
        const EntityId& unitId = storageSlotModule.UnitId();
        MemoryView<const StorageSlot> slots = storageSlotModule.StorageSlots();
        foreachitemconst(slot, slots)
        {
            if (!slot.FReservedForBuilding.Valid() && slot.Quantity == 0)
                continue;

            auto itFind = FResources.find(slot.Resource);
            if (itFind == FResources.end())
            {
                ResourceToStoragePair pair;
                pair.first = slot.Quantity;
                pair.second.insert(unitId);
                FResources.insert_or_assign(slot.Resource, pair);
            }
            else
            {
                itFind->second.first += slot.Quantity;
                itFind->second.second.insert(unitId);
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

u32 ResourceManager::GetResourceQuantity(GameResource::Type parResource) const
{
    auto itFind = FResources.find(parResource);
    if (itFind != FResources.end())
        return itFind->second.first;
    else
        return 0;
}

const ResourceToStoragePair* ResourceManager::GetStoragesForResourceIFP(GameResource::Type parResource) const
{
    auto itFind = FResources.find(parResource);
    if (itFind != FResources.end())
        return &itFind->second;
    else
        return nullptr;
}

void ResourceManager::ConsumeFromStorage(GameResource::Type parResource, u32 parQuantity, StorageSlotModule* parStorageSlotModule)
{
    auto itFind = FResources.find(parResource);
    AssertRelease(itFind != FResources.end());
    AssertRelease(itFind->second.first >= parQuantity);
    AssertRelease(itFind->second.second.find(parStorageSlotModule->UnitId()) != itFind->second.second.end());

    const u32 resourceRemoved = parStorageSlotModule->RemoveResourceInSlot(parResource, parQuantity);
    AssertRelease(resourceRemoved <= itFind->second.first);
    itFind->second.first -= resourceRemoved;
    if (parStorageSlotModule->GetNbResources(parResource) == 0)
        itFind->second.second.erase(parStorageSlotModule->UnitId());
}

void ResourceManager::ConsumeFromAnyStorage(GameResource::Type parResource, u32 parQuantity)
{
    auto itFind = FResources.find(parResource);
    AssertRelease(itFind != FResources.end());
    AssertRelease(itFind->second.first >= parQuantity);

    auto itStorage = itFind->second.second.begin();
    ModuleAccessor<StorageSlotModule> storageSlotAccessor(EEntityWorlds::BUILDINGS);
    std::vector<EntityId> emptyStorages;
    while (parQuantity > 0 && itStorage != itFind->second.second.end())
    {
        StorageSlotModule* storageSlotModule = storageSlotAccessor[*itStorage];
        const u32 resourceRemoved = storageSlotModule->RemoveResourceInSlot(parResource, parQuantity);
        AssertRelease(resourceRemoved <= itFind->second.first);
        itFind->second.first -= resourceRemoved;
        parQuantity -= resourceRemoved;
        if (storageSlotModule->GetNbResources(parResource) == 0)
            emptyStorages.push_back(*itStorage);
        ++itStorage;
    }

    reverseforeachitemconst(itToErase, emptyStorages)
    {
        itFind->second.second.erase(itToErase);
    }
}

} // namespace ECSEngine