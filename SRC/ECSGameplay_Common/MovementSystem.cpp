#include "stdafx.h"

#include "MovementSystem.h"

#include "ECSCore/ModuleAccessor.h"
#include "PositionModule.h"

namespace ECSEngine
{

MovementSystem::MovementSystem()
    : parent_type()
{
    //    RegisterDepency<PositionModule>(Worlds::STANDARD);
}

MovementSystem::~MovementSystem()
{
}

void MovementSystem::VirtualUpdate()
{
    parent_type::VirtualUpdate();
    /*ModuleAccessor<PositionModule> positionAccessor;
    foreachitem(positionModule, positionAccessor)
    {
        glm::aligned_vec3 currentPosition = positionModule.GetPosition3D();
        currentPosition += 0.02f * glm::aligned_vec3(1.0f, 0.0f, 0.0f);
        positionModule.SetPosition3D(currentPosition);
    }*/
}

void MovementSystem::VirtualInit()
{
    parent_type::VirtualInit();

    /*ModuleAccessor<PositionModule> positionAccessor;

    foreachitem(positionModule, positionAccessor) { positionModule.SetPosition3D(glm::aligned_vec3(0)); }*/
}

} // namespace ECSEngine