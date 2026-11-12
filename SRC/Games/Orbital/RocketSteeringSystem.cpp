#include "stdafx.h"

#include "RocketSteeringSystem.h"

#include "ECSCore/ModuleAccessor.h"
#include "ECSCore/WorldIds.h"
#include "Physics/PhysicsAPI.h"
#include "RocketManagerModule.h"

namespace ECSEngine
{
RocketSteeringSystem::RocketSteeringSystem()
{
    RegisterDependency<RocketManagerModule>(EEntityWorlds::STANDARD);
}

void RocketSteeringSystem::VirtualUpdate()
{
    ModuleSystem::VirtualUpdate();

    ModuleAccessor<RocketManagerModule> rocketManagerAccessor(EEntityWorlds::STANDARD);

    AssertRelease(rocketManagerAccessor.size() <= 1); // Pour l'instant on en prend qu'un !!

    foreachitemconst(rocketManager, rocketManagerAccessor)
    {
        // Handle joint orientation
        Physics::PhysicsJointHandle NozzleJointHandle = rocketManager.NozzleJointHandle();
        if (!NozzleJointHandle.IsValid())
        {
            continue;
        }

        const vec3& TargetAngle = rocketManager.TargetSteeringAngle();
        Physics::SetBallTargetEulerAngles(NozzleJointHandle, true, TargetAngle, 15.f, rocketManager.IsSteering() ? 500.f : 0.1f);
    }
}
} // namespace ECSEngine