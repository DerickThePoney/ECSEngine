
#include "stdafx.h"

#include "LinkToStorageModule.h"

#include "BuildingNeedsStorageMessgage.h"
#include "Common/GenericMessageIdentifiers.h"
#include "Common/GenericMessageManager.h"
#include "ECSCore/EntityTemplateManagerMethods.h"
#include "ECSCore/ModuleAccessor.h"
#include "ECSCore/ModuleUtils.h"
#include "ECSCore/WorldIds.h"
#include "StorageSlotModule.h"

CEREAL_REGISTER_TYPE(ECSEngine::LinkToStorageModuleTemplate);
CEREAL_REGISTER_POLYMORPHIC_RELATION(ECSEngine::ModuleTemplate, ECSEngine::LinkToStorageModuleTemplate);

namespace ECSEngine
{
IMPLEMENT_MODULE_TEMPLATE(LinkToStorageModule, LinkToStorageModuleTemplate);

Module* LinkToStorageModuleTemplate::CreateInstance(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) const
{
    return NewModule<LinkToStorageModule>(this, parUnitId, parParameters);
}

void LinkToStorageModuleTemplate::VirtualDrawEditor()
{
}

LinkToStorageModule::LinkToStorageModule()
    : Module()
{
}

LinkToStorageModule::~LinkToStorageModule()
{
}

void LinkToStorageModule::OnUnitDeath(const EntityId parId)
{
    if (parId == FStorageId)
    {
        FStorageId = EntityId();

        BuildingNeedsStorageMessage* message = new BuildingNeedsStorageMessage();
        message->FUnitId = UnitId();
        GenericMessageManager::Instance().PushMessage<GenericMessageId::BUILDING_NEEDS_STORAGE>(message);
    }
}

void LinkToStorageModule::VirtualInit(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters)
{
    Module::VirtualInit(parUnitId, parParameters);

    WorldManager::Instance().RegisterDeathListener(DELEGATE(&LinkToStorageModule::OnUnitDeath, *this));

    BuildingNeedsStorageMessage* message = new BuildingNeedsStorageMessage();
    message->FUnitId = UnitId();
    GenericMessageManager::Instance().PushMessage<GenericMessageId::BUILDING_NEEDS_STORAGE>(message);
}

void LinkToStorageModule::VirtualDeinit()
{
    Module::VirtualDeinit();

    WorldManager::Instance().RemoveDeathListener(DELEGATE(&LinkToStorageModule::OnUnitDeath, *this));

    if (FStorageId.Valid())
    {
        ManualLockModuleAccessor<StorageSlotModule> buildingAccessor(EEntityWorlds::BUILDINGS);
        buildingAccessor.LockIFN();
        StorageSlotModule* slotModule = buildingAccessor[FStorageId];
        buildingAccessor.UnlockIFN();
        if (slotModule != nullptr)
        {
            slotModule->RemoveSlotsReservationsIFN(UnitId());
        }
    }
}

} // namespace ECSEngine
