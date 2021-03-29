#include "stdafx.h"

#include "OrientationSystem.h"

#include "Common/TimeManager.h"
#include "ECSCore/ModuleAccessor.h"
#include "OrientationModule.h"

namespace ECSEngine
{

OrientationSystem::OrientationSystem()
{
    RegisterDepency<OrientationModule>(Worlds::STANDARD);
}

OrientationSystem::~OrientationSystem()
{
}

void OrientationSystem::VirtualUpdate()
{
    parent_type::VirtualUpdate();
    const float timepoint = TimeManager::FrameDeltaTime();

    const glm::quat rotate(glm::vec3(timepoint, timepoint, 0.f));

    ModuleAccessor<OrientationModule> orientationAccessor;

    foreachitem(orientationModule, orientationAccessor)
    {
        const glm::quat currentOrientation = orientationModule.GetOrientation();
        const glm::quat newOrientation = rotate * currentOrientation;
        orientationModule.SetOrientation(newOrientation);
    }
}

} // namespace ECSEngine
