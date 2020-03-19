#include "stdafx.h"

#include "OrientationModule.h"

#include "ECSCore/EntityTemplateManager.h"
#include "ECSCore/ModuleParameters.h"
#include "ECSCore/ModuleUtils.h"

CEREAL_REGISTER_TYPE(ECSEngine::OrientationModuleTemplate);
CEREAL_REGISTER_POLYMORPHIC_RELATION(ECSEngine::ModuleTemplate, ECSEngine::OrientationModuleTemplate)

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

const glm::vec3 OrientationModule::Forward() const
{
    glm::mat4 rotationMatrix(FOrientation);
    return glm::vec3(rotationMatrix[0]);
}

const glm::vec3 OrientationModule::Right() const
{
    glm::mat4 rotationMatrix(FOrientation);
    return glm::vec3(rotationMatrix[1]);
}

const glm::vec3 OrientationModule::Up() const
{
    glm::mat4 rotationMatrix(FOrientation);
    return glm::vec3(rotationMatrix[2]);
}

} // namespace ECSEngine