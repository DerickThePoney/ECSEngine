#include "stdafx.h"

#include "ResourceProductionModule.h"

#include "Application/PropertyDrawer.h"
#include "ECSCore/EntityId.h"
#include "ECSCore/EntityTemplateManager.h"
#include "ECSCore/ModuleParameters.h"
#include "ECSCore/ModuleUtils.h"

CEREAL_REGISTER_TYPE(ECSEngine::ResourceProductionModuleTemplate);
CEREAL_REGISTER_POLYMORPHIC_RELATION(ECSEngine::ModuleTemplate, ECSEngine::ResourceProductionModuleTemplate)

namespace ECSEngine
{
IMPLEMENT_MODULE_TEMPLATE(ResourceProductionModule, ResourceProductionModuleTemplate);

Module* ResourceProductionModuleTemplate::CreateInstance(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) const
{
    return NewModule<ResourceProductionModule>(this, parUnitId, parParameters);
}

void ResourceProductionModuleTemplate::VirtualDrawEditor()
{
}

ResourceProductionModule::ResourceProductionModule()
    : Module()
{
}

void ResourceProductionModule::VirtualInit(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters)
{
}

} // namespace ECSEngine
