
#include "stdafx.h"

#include "LinkToWorkPlaceModule.h"

#include "ECSCore/EntityTemplateManager.h"
#include "ECSCore/ModuleUtils.h"

CEREAL_REGISTER_TYPE(ECSEngine::LinkToWorkPlaceModuleTemplate);
CEREAL_REGISTER_POLYMORPHIC_RELATION(ECSEngine::ModuleTemplate, ECSEngine::LinkToWorkPlaceModuleTemplate);

namespace ECSEngine
{
IMPLEMENT_MODULE_TEMPLATE(LinkToWorkPlaceModule, LinkToWorkPlaceModuleTemplate);

Module* LinkToWorkPlaceModuleTemplate::CreateInstance(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) const
{
    return NewModule<LinkToWorkPlaceModule>(this, parUnitId, parParameters);
}

void LinkToWorkPlaceModuleTemplate::VirtualDrawEditor()
{
}

LinkToWorkPlaceModule::LinkToWorkPlaceModule()
    : Module()
{
}

LinkToWorkPlaceModule::~LinkToWorkPlaceModule()
{
}

} // namespace ECSEngine
