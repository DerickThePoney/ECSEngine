
#include "stdafx.h"

#include "LinkToHousingPlaceModule.h"

#include "ECSCore/EntityTemplateManager.h"
#include "ECSCore/ModuleUtils.h"

CEREAL_REGISTER_TYPE(ECSEngine::LinkToHousingPlaceModuleTemplate);
CEREAL_REGISTER_POLYMORPHIC_RELATION(ECSEngine::ModuleTemplate, ECSEngine::LinkToHousingPlaceModuleTemplate);

namespace ECSEngine
{
IMPLEMENT_MODULE_TEMPLATE(LinkToHousingPlaceModule, LinkToHousingPlaceModuleTemplate);

Module* LinkToHousingPlaceModuleTemplate::CreateInstance(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) const
{
    return NewModule<LinkToHousingPlaceModule>(this, parUnitId, parParameters);
}

void LinkToHousingPlaceModuleTemplate::VirtualDrawEditor()
{
}

LinkToHousingPlaceModule::LinkToHousingPlaceModule()
    : Module()
{
}

LinkToHousingPlaceModule::~LinkToHousingPlaceModule()
{
}

} // namespace ECSEngine
