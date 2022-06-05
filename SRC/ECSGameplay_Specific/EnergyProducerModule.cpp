
#include "stdafx.h"

#include "EnergyProducerModule.h"

#include "Application/PropertyDrawer.h"
#include "ECSCore/EntityTemplateManagerMethods.h"
#include "ECSCore/ModuleUtils.h"

CEREAL_REGISTER_TYPE(ECSEngine::EnergyProducerModuleTemplate);
CEREAL_REGISTER_POLYMORPHIC_RELATION(ECSEngine::ModuleTemplate, ECSEngine::EnergyProducerModuleTemplate);

namespace ECSEngine
{
IMPLEMENT_MODULE_TEMPLATE(EnergyProducerModule, EnergyProducerModuleTemplate);

Module* EnergyProducerModuleTemplate::CreateInstance(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) const
{
    return NewModule<EnergyProducerModule>(this, parUnitId, parParameters);
}

void EnergyProducerModuleTemplate::VirtualDrawEditor()
{
    EDITOR_PROPERTY_SIMPLE("Produced energy", FProducedEnergy);
}

EnergyProducerModule::EnergyProducerModule()
    : Module()
{
}

EnergyProducerModule::~EnergyProducerModule()
{
}

u32 EnergyProducerModule::ProducedEnergy() const
{
    const auto* moduleTemplate = Template<EnergyProducerModuleTemplate>();
    AssertRelease(moduleTemplate != nullptr);
    return moduleTemplate->ProducedEnergy();
}

} // namespace ECSEngine
