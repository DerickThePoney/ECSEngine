#include "stdafx.h"

#include "StorageSlotSystem.h"

#include "BuildingNeedsStorageMessgage.h"
#include "Common/GenericMessageIdentifiers.h"
#include "Common/GenericMessageManager.h"
#include "Common/MemoryView.h"
#include "ECSCore/ModuleAccessor.h"
#include "ECSGameplay_Common/PositionModule.h"
#include "LinkToStorageModule.h"
#include "RawResourceProductionModule.h"
#include "StorageSlotModule.h"

namespace ECSEngine
{
StorageSlotSystem::StorageSlotSystem()
    : ModuleSystem()
{
    RegisterDepency<StorageSlotModule>(Worlds::BUILDINGS);
    RegisterDepency<LinkToStorageModule>(Worlds::BUILDINGS);
    RegisterDepency<PositionModule>(Worlds::BUILDINGS);
    RegisterDepency<RawResourceProductionModule>(Worlds::BUILDINGS);
}

void StorageSlotSystem::VirtualUpdate()
{
    ModuleSystem::VirtualUpdate();

    ModuleAccessor<StorageSlotModule> storageSlotAccessor(Worlds::BUILDINGS);
    ModuleAccessor<LinkToStorageModule> linkToStorageAccessor(Worlds::BUILDINGS);
    ModuleAccessor<PositionModule> positionAccessor(Worlds::BUILDINGS);
    ModuleAccessor<RawResourceProductionModule> rawProdAccessor(Worlds::BUILDINGS);

    auto functor = [this, &storageSlotAccessor, &positionAccessor, &linkToStorageAccessor, &rawProdAccessor](const BuildingNeedsStorageMessage& parMessage) {
        this->ProcessMessages(parMessage, storageSlotAccessor, positionAccessor, linkToStorageAccessor, rawProdAccessor);
    };
    GenericMessageManager::Instance().ProcessMessages<GenericMessageId::BUILDING_NEEDS_STORAGE, BuildingNeedsStorageMessage>(functor);
}

void StorageSlotSystem::ProcessMessages(const BuildingNeedsStorageMessage& parMessage,
      ModuleAccessor<StorageSlotModule>& parStorageSlotAccessor,
      ModuleAccessor<PositionModule>& parPositionModuleAccessor,
      ModuleAccessor<LinkToStorageModule>& parLinkToStorageAccessor,
      ModuleAccessor<RawResourceProductionModule>& parRawProductionAccessor)
{
    LinkToStorageModule* linkToStorageForEntity = parLinkToStorageAccessor[parMessage.FUnitId];
    AssertRelease(linkToStorageForEntity != nullptr);
    const PositionModule* positionModuleForEntity = parPositionModuleAccessor[parMessage.FUnitId];
    AssertRelease(positionModuleForEntity != nullptr);

    std::vector<GameResource::Type> resourcesToReserve;

    const RawResourceProductionModule* rawProdModuleForEntity = parRawProductionAccessor[parMessage.FUnitId];
    if (rawProdModuleForEntity)
    {
        auto producedRes = rawProdModuleForEntity->ProducedResourcesTimings();
        foreachitemconst(res, producedRes) { resourcesToReserve.push_back(res.first); }
    }

    AlwaysCheckedAssert(!resourcesToReserve.empty());

    foreachitem(storage, parStorageSlotAccessor)
    {
        if (storage.FreeSlots() < resourcesToReserve.size())
            continue;

        const PositionModule* positionModuleForStorage = parPositionModuleAccessor[storage.UnitId()];
        AssertRelease(positionModuleForStorage != nullptr);
        const float radiusSq = storage.RadiusOfEffect() * storage.RadiusOfEffect();
        const float distanceSq = glm::length2(glm::xy(positionModuleForEntity->GetPosition3D() - positionModuleForStorage->GetPosition3D()));
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

} // namespace ECSEngine
