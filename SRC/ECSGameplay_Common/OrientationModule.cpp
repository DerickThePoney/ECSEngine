#include "stdafx.h"

#include "OrientationModule.h"

#include "ECSCore/EntityTemplateManager.h"
#include "ECSCore/ModuleParameters.h"
#include "ECSCore/ModuleUtils.h"

namespace ECSEngine
{

IMPLEMENT_MODULE_TEMPLATE(OrientationModule, OrientationModuleTemplate);

Module* OrientationModuleTemplate::CreateInstance(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) const
{
    return NewModule<OrientationModule>(parUnitId, parParameters);
}

void OrientationModule::VirtualInit(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters)
{
    parent_type::VirtualInit(parUnitId, parParameters);

    const glm::vec3 yawPitchRoll = parParameters.Get_IFP<ModuleParameters::Orientation>(glm::vec3(0.0f));
    FOrientation = glm::quat(yawPitchRoll);
}

const glm::vec3 OrientationModule::GetOrientationAsYawPitchRoll() const
{
    return glm::vec3(glm::yaw(FOrientation), glm::pitch(FOrientation), glm::roll(FOrientation));
}

} // namespace ECSEngine