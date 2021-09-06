#include "stdafx.h"

#include "StorageSlotSystem.h"

#include "BuildingNeedsStorageMessgage.h"
#include "Common/GenericMessageIdentifiers.h"
#include "Common/GenericMessageManager.h"

namespace ECSEngine
{
StorageSlotSystem::StorageSlotSystem()
    : ModuleSystem()
{
}

void StorageSlotSystem::VirtualUpdate()
{
    ModuleSystem::VirtualUpdate();

    auto functor = [this](const BuildingNeedsStorageMessage& parMessage) { this->ProcessMessages(parMessage); };
    GenericMessageManager::Instance().ProcessMessages<GenericMessageId::BUILDING_NEEDS_STORAGE, BuildingNeedsStorageMessage>(
          functor);
}

void StorageSlotSystem::ProcessMessages(const BuildingNeedsStorageMessage& parMessage)
{
}

} // namespace ECSEngine
