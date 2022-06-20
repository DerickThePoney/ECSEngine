
#include "stdafx.h"

#include "EnergyConsumerModule.h"

#include "Application/PropertyDrawer.h"
#include "Common/SavingSystemImplementation.h"
#include "ECSCore/EntityTemplateManagerMethods.h"
#include "ECSCore/ModuleUtils.h"

CEREAL_REGISTER_TYPE(ECSEngine::EnergyConsumerModuleTemplate);
CEREAL_REGISTER_POLYMORPHIC_RELATION(ECSEngine::ModuleTemplate, ECSEngine::EnergyConsumerModuleTemplate);

namespace ECSEngine
{
IMPLEMENT_MODULE_TEMPLATE(EnergyConsumerModule, EnergyConsumerModuleTemplate);

Module* EnergyConsumerModuleTemplate::CreateInstance(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) const
{
    return NewModule<EnergyConsumerModule>(this, parUnitId, parParameters);
}

void EnergyConsumerModuleTemplate::VirtualDrawEditor()
{
    EDITOR_PROPERTY_SIMPLE("Consumed energy", FConsumedEnergy);
}

EnergyConsumerModule::EnergyConsumerModule()
    : Module()
{
}

EnergyConsumerModule::~EnergyConsumerModule()
{
}

u32 EnergyConsumerModule::ConsumedEnergy() const
{
    const auto* moduleTemplate = Template<EnergyConsumerModuleTemplate>();
    AssertRelease(moduleTemplate != nullptr);
    return moduleTemplate->ConsumedEnergy();
}

IMPLEMENT_SAVELOAD_ABILITIES(EnergyConsumerModule);
template<typename Chunk, bool isWriting>
void EnergyConsumerModule::SaveLoad(Chunk& parChunk)
{
    parent_type::SaveLoad(parChunk);
}

} // namespace ECSEngine
