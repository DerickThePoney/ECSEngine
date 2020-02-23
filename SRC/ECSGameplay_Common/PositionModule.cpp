#include "stdafx.h"

#include "PositionModule.h"

#include "ECSCore/EntityTemplateManager.h"
#include "ECSCore/ModuleParameters.h"
#include "ECSCore/ModuleUtils.h"

namespace ECSEngine
{

IMPLEMENT_MODULE_TEMPLATE(PositionModule, PositionModuleTemplate);

Module* PositionModuleTemplate::CreateInstance(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) const
{
    return NewModule<PositionModule>(parUnitId, parParameters);
}

void PositionModule::VirtualInit(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters)
{
    parent_type::VirtualInit(parUnitId, parParameters);

    FPosition = parParameters.Get_IFP<ModuleParameters::Position>(glm::vec3(0.0f));
}

} // namespace ECSEngine
