#include "stdafx.h"

#include "ResourceHarvesterModule.h"

#include "Application/PropertyDrawer.h"
#include "ECSCore/EntityTemplateManager.h"
#include "ECSCore/ModuleParameters.h"
#include "ECSCore/ModuleUtils.h"

CEREAL_REGISTER_TYPE(ECSEngine::ResourceHarvesterModuleTemplate);
CEREAL_REGISTER_POLYMORPHIC_RELATION(ECSEngine::ModuleTemplate, ECSEngine::ResourceHarvesterModuleTemplate);

namespace ECSEngine
{
IMPLEMENT_MODULE_TEMPLATE(ResourceHarvesterModule, ResourceHarvesterModuleTemplate);

Module* ResourceHarvesterModuleTemplate::CreateInstance(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) const
{
    return NewModule<ResourceHarvesterModule>(this, parUnitId, parParameters);
}

void ResourceHarvesterModuleTemplate::VirtualDrawEditor()
{
    EDITOR_PROPERTY_WITH_LIMITS("Harvesting duration", FHarvestingDuration, 0.f, 10.f);
}

void ResourceHarvesterModule::VirtualInit(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parContainer)
{
    parent_type::VirtualInit(parUnitId, parContainer);
}

} // namespace ECSEngine
