#include "stdafx.h"

#include "MovementModule.h"

#include "Application/PropertyDrawer.h"
#include "ECSCore/EntityId.h"
#include "ECSCore/EntityTemplateManager.h"
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

} // namespace ECSEngine