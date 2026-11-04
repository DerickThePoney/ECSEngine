#include "stdafx.h"

#include "RocketThrustSystem.h"

#include "ECSCore/ModuleAccessor.h"
#include "ECSCore/WorldIds.h"
#include "ECSGameplay_Common/RigidbodyModule.h"
#include "Physics/PhysicsAPI.h"
#include "RocketManagerModule.h"

namespace ECSEngine
{
RocketThrustSystem::RocketThrustSystem()
{
    RegisterDependency<RocketManagerModule>(EEntityWorlds::STANDARD);
    RegisterDependency<RigidbodyModule>(EEntityWorlds::STANDARD);
}

void RocketThrustSystem::VirtualUpdate()
{
    ModuleSystem::VirtualUpdate();

    ModuleAccessor<RocketManagerModule> rocketManagerAccessor(EEntityWorlds::STANDARD);
    ModuleAccessor<RigidbodyModule> rigidbodyAccessor(EEntityWorlds::STANDARD);

    AssertRelease(rocketManagerAccessor.size() <= 1); // Pour l'instant on en prend qu'un !!

    foreachitemconst(rocketManager, rocketManagerAccessor)
    {
        if (!rocketManager.EngineFiring())
        {
            continue;
        }

        // TODO - Set the propulsion force on an engine module put on the nozzle.
        RigidbodyModule* NozzleBody = rigidbodyAccessor[rocketManager.NozzleId()];
        AssertRelease(NozzleBody != nullptr);
        quat BodyOrientation;
        Physics::GetBodyOrientation(NozzleBody->BodyHandle(), BodyOrientation);
        vec3 PropulsionForce = (mat4(BodyOrientation) * vec4(0.f, 50.f, 0.f, 0.f)).xyz();
        Physics::AddForceToBody(NozzleBody->BodyHandle(), PropulsionForce, false);
    }
}
} // namespace ECSEngine