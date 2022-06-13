#include "stdafx.h"

#include "StorageSlotSystem.h"

#include "BuildingNeedsStorageMessgage.h"
#include "Common/GenericMessageIdentifiers.h"
#include "Common/GenericMessageManager.h"
#include "Common/MemoryView.h"
#include "ECSCore/ModuleAccessor.h"
#include "ECSCore/WorldIds.h"
#include "ECSGameplay_Common/PositionModule.h"
#include "LinkToStorageModule.h"
#include "RecipeProductionModule.h"
#include "ResourceManager.h"
#include "ResourceStorageModule.h"
#include "StorageSlotModule.h"

namespace ECSEngine
{
StorageSlotSystem::StorageSlotSystem()
    : ModuleSystem()
{
    RegisterDepency<StorageSlotModule>(EEntityWorlds::BUILDINGS);
    RegisterDepency<ResourceStorageModule>(EEntityWorlds::BUILDINGS);
    RegisterDepency<LinkToStorageModule>(EEntityWorlds::BUILDINGS);
    RegisterDepency<PositionModule>(EEntityWorlds::BUILDINGS);
    RegisterDepency<RecipeProductionModule>(EEntityWorlds::BUILDINGS);
}

void StorageSlotSystem::VirtualUpdate()
{
    ModuleSystem::VirtualUpdate();

    ModuleAccessor<StorageSlotModule> storageSlotAccessor(EEntityWorlds::BUILDINGS);
    ModuleAccessor<ResourceStorageModule> resourceStorageAccessor(EEntityWorlds::BUILDINGS);
    ModuleAccessor<LinkToStorageModule> linkToStorageAccessor(EEntityWorlds::BUILDINGS);
    ModuleAccessor<PositionModule> positionAccessor(EEntityWorlds::BUILDINGS);
    ModuleAccessor<RecipeProductionModule> recipeProdAccessor(EEntityWorlds::BUILDINGS);

    auto functor = [this, &storageSlotAccessor, &positionAccessor, &linkToStorageAccessor, &recipeProdAccessor](const BuildingNeedsStorageMessage& parMessage)
    { this->ProcessMessages(parMessage, storageSlotAccessor, positionAccessor, linkToStorageAccessor, recipeProdAccessor); };
    GenericMessageManager::Instance().ProcessMessages<GenericMessageId::BUILDING_NEEDS_STORAGE, BuildingNeedsStorageMessage>(functor);

    TransfertResourcesFromProducersToStorage(resourceStorageAccessor, storageSlotAccessor, linkToStorageAccessor, recipeProdAccessor);

    TransfertResourcesFromStoragesToProducers(resourceStorageAccessor, storageSlotAccessor, linkToStorageAccessor, recipeProdAccessor);
}

void StorageSlotSystem::ProcessMessages(const BuildingNeedsStorageMessage& parMessage,
      ModuleAccessor<StorageSlotModule>& parStorageSlotAccessor,
      ModuleAccessor<PositionModule>& parPositionModuleAccessor,
      ModuleAccessor<LinkToStorageModule>& parLinkToStorageAccessor,
      ModuleAccessor<RecipeProductionModule>& parRecipeProductionAccessor)
{
    LinkToStorageModule* linkToStorageForEntity = parLinkToStorageAccessor[parMessage.FUnitId];
    AssertRelease(linkToStorageForEntity != nullptr);
    const PositionModule* positionModuleForEntity = parPositionModuleAccessor[parMessage.FUnitId];
    AssertRelease(positionModuleForEntity != nullptr);

    std::vector<GameResource::Type> resourcesToReserve;

    const RecipeProductionModule* recipeProdModuleForEntity = parRecipeProductionAccessor[parMessage.FUnitId];
    if (recipeProdModuleForEntity != nullptr)
    {
        const ProductionRecipe* recipe = recipeProdModuleForEntity->GetProductionRecipe();
        AlwaysCheckedAssert(recipe != nullptr);
        if (recipe != nullptr)
        {
            MemoryView<const RecipeComponent> outputResources = recipe->OutputComponents();
            foreachitemconst(res, outputResources) { resourcesToReserve.push_back(res.first); }
        }
    }

    AlwaysCheckedAssert(!resourcesToReserve.empty());
    if (resourcesToReserve.empty())
        return;

    foreachitem(storage, parStorageSlotAccessor)
    {
        if (storage.FreeSlots() < resourcesToReserve.size())
            continue;

        const PositionModule* positionModuleForStorage = parPositionModuleAccessor[storage.UnitId()];
        AssertRelease(positionModuleForStorage != nullptr);
        const float radiusSq = storage.RadiusOfEffect() * storage.RadiusOfEffect();
        const float distanceSq = glm::length2(glm::xz(positionModuleForEntity->GetPosition3D() - positionModuleForStorage->GetPosition3D()));
        if (distanceSq <= radiusSq)
        {
            const bool success = storage.ReserveSlotsIFP(parMessage.FUnitId, MemoryView<const GameResource::Type>(resourcesToReserve.data(), resourcesToReserve.size()));
            if (success)
            {
                linkToStorageForEntity->SetStorageId(storage.UnitId());
                return;
            }
        }
    }

    // If we arrive here, we got no storage, so push back the message on the stack for next update
    BuildingNeedsStorageMessage* newMessage = new BuildingNeedsStorageMessage(parMessage);
    GenericMessageManager::Instance().PushMessage<GenericMessageId::BUILDING_NEEDS_STORAGE>(newMessage);
}

void StorageSlotSystem::TransfertResourcesFromProducersToStorage(ModuleAccessor<ResourceStorageModule>& parResourceStorageAccessor,
      ModuleAccessor<StorageSlotModule>& parStorageSlotAccessor,
      ModuleAccessor<LinkToStorageModule>& parLinkToStorageAccessor,
      ModuleAccessor<RecipeProductionModule>& parRecipeProductionAccessor)
{
    foreachitemconst(recipeProducer, parRecipeProductionAccessor)
    {
        const ProductionRecipe* recipe = recipeProducer.GetProductionRecipe();
        AlwaysCheckedAssert(recipe != nullptr);
        if (recipe == nullptr)
            continue;

        MemoryView<const RecipeComponent> producedRes = recipe->OutputComponents();
        ResourceStorageModule* resStorage = parResourceStorageAccessor[recipeProducer.UnitId()];
        AssertRelease(resStorage != nullptr);
        const LinkToStorageModule* linkToStorage = parLinkToStorageAccessor[recipeProducer.UnitId()];
        AssertRelease(linkToStorage != nullptr);

        const EntityId storage = linkToStorage->StorageId();
        if (!storage.Valid())
            continue;

        StorageSlotModule* storageSlotModule = parStorageSlotAccessor[storage];
        AssertRelease(storageSlotModule != nullptr);

        foreachitemconst(res, producedRes)
        {
            const u32 resQ = resStorage->GetResourceQuantity(res.first);
            if (resQ > 0)
            {
                const u32 maxFreeSpaceInStorage = storageSlotModule->GetFreeSpaceInSlot(recipeProducer.UnitId(), res.first);
                if (maxFreeSpaceInStorage > 0)
                {
                    const u32 resRemoved = resStorage->RemoveResource(res.first, glm::min(resQ, maxFreeSpaceInStorage));
                    storageSlotModule->AddResourceInSlot(recipeProducer.UnitId(), res.first, resRemoved);
                }
            }
        }
    }
}

void StorageSlotSystem::TransfertResourcesFromStoragesToProducers(ModuleAccessor<ResourceStorageModule>& parResourceStorageAccessor,
      ModuleAccessor<StorageSlotModule>& parStorageSlotAccessor,
      ModuleAccessor<LinkToStorageModule>& parLinkToStorageAccessor,
      ModuleAccessor<RecipeProductionModule>& parRecipeProductionAccessor)
{
    foreachitemconst(recipeProducer, parRecipeProductionAccessor)
    {
        const ProductionRecipe* recipe = recipeProducer.GetProductionRecipe();
        AlwaysCheckedAssert(recipe != nullptr);
        if (recipe == nullptr)
            continue;

        MemoryView<const RecipeComponent> inputResources = recipe->InputComponents();
        ResourceStorageModule* resStorage = parResourceStorageAccessor[recipeProducer.UnitId()];
        AssertRelease(resStorage != nullptr);

        u32 wantedResourceQty = 0;
        bool canGetEveryThing = true;
        foreachitemconst(inputResource, inputResources)
        {
            wantedResourceQty += inputResource.second;
            if (ResourceManager::Instance().GetResourceQuantity(inputResource.first) < inputResource.second)
            {
                canGetEveryThing = false;
                break;
            }
        }

        const u32 outputResourceQty = recipe->TotalQuantityOfOutputResourcesNecessary();

        if (resStorage->GetRemainingStorageSpace() < (wantedResourceQty + outputResourceQty) || !canGetEveryThing)
            continue;

        foreachitemconst(inputResource, inputResources)
        {
            u32 resourcesToGet = inputResource.second;
            while (resourcesToGet > 0)
            {
                const auto storages = ResourceManager::Instance().GetStoragesForResourceIFP(inputResource.first);
                AssertRelease(!storages->second.empty());
                const EntityId& storage = *storages->second.begin();
                StorageSlotModule* slotModule = parStorageSlotAccessor[storage];
                AssertRelease(slotModule != nullptr);
                const u32 quantityInSlots = slotModule->GetNbResources(inputResource.first);
                const u32 qtyToGet = glm::min(quantityInSlots, resourcesToGet);

                ResourceManager::Instance().ConsumeFromStorage(inputResource.first, qtyToGet, slotModule);
                resourcesToGet -= qtyToGet;
            }
            resStorage->AddResource(inputResource.first, inputResource.second);
        }
    }
}

} // namespace ECSEngine
