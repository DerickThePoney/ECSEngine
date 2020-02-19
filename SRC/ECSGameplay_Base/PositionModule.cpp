#include "stdafx.h"

#include "PositionModule.h"

#include "ECSBase/ModuleParameters.h"

namespace ECSEngine
{

void PositionModule::VirtualInit(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters)
{
    parent_type::VirtualInit(parUnitId, parParameters);

    FPosition = parParameters.Get_IFP<ModuleParameters::Position>(glm::vec3(0.0f));
}

} // namespace ECSEngine
