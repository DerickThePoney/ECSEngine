#include "stdafx.h"

#include "PositionModule.h"

#include "ModuleParameters.h"

namespace ECSEngine
{

void PositionModule::VirtualInit(const ModuleParameters::ParameterContainer& parParameters)
{
    parent_type::VirtualInit(parParameters);

    FPosition = parParameters.Get_IFP<ModuleParameters::Position>(glm::vec3(0.0f));
}

} // namespace ECSEngine
