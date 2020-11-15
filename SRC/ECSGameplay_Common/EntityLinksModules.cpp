#include "stdafx.h"

#include "EntityLinksModules.h"

#include "ECSCore/EntityId.h"
#include "ECSCore/EntityTemplateManager.h"
#include "ECSCore/ModuleParameters.h"
#include "ECSCore/ModuleUtils.h"

CEREAL_REGISTER_TYPE(ECSEngine::LinkToOwnerModuleTemplate);
CEREAL_REGISTER_POLYMORPHIC_RELATION(ECSEngine::ModuleTemplate, ECSEngine::LinkToOwnerModuleTemplate);

namespace ECSEngine
{
IMPLEMENT_MODULE_TEMPLATE(LinkToOwnerModule, LinkToOwnerModuleTemplate);

Module* LinkToOwnerModuleTemplate::CreateInstance(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) const
{
    return NewModule<LinkToOwnerModule>(this, parUnitId, parParameters);
}

void LinkToOwnerModuleTemplate::VirtualDrawEditor()
{
}

void LinkToOwnerModule::VirtualInit(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters)
{
    parent_type::VirtualInit(parUnitId, parParameters);

    FOwnerId = parParameters.Get_IFP<ModuleParameters::OwnerId>(EntityId());
}

} // namespace ECSEngine
