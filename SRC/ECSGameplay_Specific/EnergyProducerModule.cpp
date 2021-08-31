
#include "stdafx.h"

#include "EnergyProducerModule.h"

#include "ECSCore/EntityTemplateManager.h"
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
}

EnergyProducerModule::EnergyProducerModule()
    : Module()
{
}

EnergyProducerModule::~EnergyProducerModule()
{
}

} // namespace ECSEngine
