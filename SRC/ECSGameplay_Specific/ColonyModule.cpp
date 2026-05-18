#include "stdafx.h"

#include "ColonyModule.h"

#include "ECSCore/EntityId.h"
#include "ECSCore/EntityTemplateManagerMethods.h"
#include "ECSCore/ModuleParameters.h"
#include "ECSCore/ModuleUtils.h"

CEREAL_REGISTER_TYPE(ECSEngine::ColonyModuleTemplate);
CEREAL_REGISTER_POLYMORPHIC_RELATION(ECSEngine::ModuleTemplate, ECSEngine::ColonyModuleTemplate)

namespace ECSEngine
{
IMPLEMENT_MODULE_TEMPLATE(ColonyModule, ColonyModuleTemplate);

Module* ColonyModuleTemplate::CreateInstance(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) const
{
    return NewModule<ColonyModule>(this, parUnitId, parParameters);
}

void ColonyModuleTemplate::VirtualDrawEditor()
{
}

ColonyModule::ColonyModule()
    : Module()
{
}

void ColonyModule::VirtualInit(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters)
{
    parent_type::VirtualInit(parUnitId, parParameters);

    FName = std::format("Colony_{}", parUnitId.GetSequentialId());
}

} // namespace ECSEngine
