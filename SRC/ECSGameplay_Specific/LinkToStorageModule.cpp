
#include "stdafx.h"

#include "LinkToStorageModule.h"

#include "BuildingNeedsStorageMessgage.h"
#include "Common/GenericMessageIdentifiers.h"
#include "Common/GenericMessageManager.h"
#include "ECSCore/EntityTemplateManagerMethods.h"
#include "ECSCore/ModuleUtils.h"

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

void LinkToStorageModule::VirtualInit(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters)
{
    Module::VirtualInit(parUnitId, parParameters);

    BuildingNeedsStorageMessage* message = new BuildingNeedsStorageMessage();
    message->FUnitId = UnitId();
    GenericMessageManager::Instance().PushMessage<GenericMessageId::BUILDING_NEEDS_STORAGE>(message);
}

} // namespace ECSEngine
