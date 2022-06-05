#include "stdafx.h"

#include "MovementModule.h"

#include "Application/PropertyDrawer.h"
#include "ECSCore/EntityId.h"
#include "ECSCore/EntityTemplateManagerMethods.h"
#include "ECSCore/ModuleParameters.h"
#include "ECSCore/ModuleUtils.h"

CEREAL_REGISTER_TYPE(ECSEngine::MovementModuleTemplate);
CEREAL_REGISTER_POLYMORPHIC_RELATION(ECSEngine::ModuleTemplate, ECSEngine::MovementModuleTemplate)
namespace ECSEngine
{
IMPLEMENT_MODULE_TEMPLATE(MovementModule, MovementModuleTemplate);

Module* MovementModuleTemplate::CreateInstance(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) const
{
    return NewModule<MovementModule>(this, parUnitId, parParameters);
}

void MovementModuleTemplate::VirtualDrawEditor()
{
    EDITOR_PROPERTY_SIMPLE("Max speed", FMaxSpeed);
    if (FMaxSpeed < 0.f)
        FMaxSpeed = 0.f;

    EDITOR_PROPERTY_ANGLE("Max rotation speed", FMaxRotationSpeed, 0, 2000);
}

MovementModule::MovementModule()
    : Module()
{
}

void MovementModule::VirtualInit(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters)
{
    parent_type::VirtualInit(parUnitId, parParameters);
}

void MovementModule::SetNewPath(const std::vector<glm::vec2>& parNewPath)
{
    AlwaysCheckedAssert(parNewPath.size() > 0);
    FPath = parNewPath;
    FCurrentFollowedWaypoint = 0;
}

const glm::vec3* MovementModule::GetPathForDebug(const glm::vec3& parPosition, u32& outSize)
{
    FPathForDebug.clear();

    outSize = 0;
    if (FCurrentFollowedWaypoint == -1 || FPath.size() == 0)
        return nullptr;

    FPathForDebug.push_back(parPosition);
    forrange(i, FCurrentFollowedWaypoint, FPath.size()) { FPathForDebug.push_back(glm::vec3(FPath[i].x, 0.f, FPath[i].y)); }
    outSize = (u32)FPathForDebug.size();
    return FPathForDebug.data();
}

void MovementModule::ClearPath()
{
    FPath.clear();
    FCurrentFollowedWaypoint = 0;
    FCurrentSpeed = glm::vec3(0.f);
}

} // namespace ECSEngine
