#include "stdafx.h"

#include "RocketThrustSystem.h"

#include "ECSCore/ModuleAccessor.h"
#include "ECSCore/WorldIds.h"
#include "ECSGameplay_Common/RigidbodyModule.h"
#include "NozzleEngineModule.h"
#include "Physics/PhysicsAPI.h"
#include "RocketManagerModule.h"

namespace ECSEngine
{
RocketThrustSystem::RocketThrustSystem()
{
    RegisterDependency<RocketManagerModule>(EEntityWorlds::STANDARD);
    RegisterDependency<RigidbodyModule>(EEntityWorlds::STANDARD);
    RegisterDependency<NozzleEngineModule>(EEntityWorlds::STANDARD);
}

void RocketThrustSystem::VirtualUpdate()
{
    ModuleSystem::VirtualUpdate();

    ModuleAccessor<RocketManagerModule> rocketManagerAccessor(EEntityWorlds::STANDARD);
    ModuleAccessor<RigidbodyModule> rigidbodyAccessor(EEntityWorlds::STANDARD);
    ModuleAccessor<NozzleEngineModule> nozzleEngineAccessor(EEntityWorlds::STANDARD);

    AssertRelease(rocketManagerAccessor.size() <= 1); // Pour l'instant on en prend qu'un !!

    foreachitemconst(rocketManager, rocketManagerAccessor)
    {
        NozzleEngineModule* nozzleEngine = nozzleEngineAccessor[rocketManager.NozzleId()];
        AssertRelease(nozzleEngine != nullptr);

        nozzleEngine->SetThrottle(rocketManager.RocketThrottle());

        if (nozzleEngine->Throttle() <= 0.f)
        {
            continue;
        }

        RigidbodyModule* NozzleBody = rigidbodyAccessor[rocketManager.NozzleId()];
        AssertRelease(NozzleBody != nullptr);
        quat BodyOrientation;
        Physics::GetBodyOrientation(NozzleBody->BodyHandle(), BodyOrientation);

        const float Prop = nozzleEngine->Throttle() * nozzleEngine->MaxThrust();
        vec3 PropulsionForce = (mat4(BodyOrientation) * vec4(0.f, Prop, 0.f, 0.f)).xyz();
        Physics::AddForceToBody(NozzleBody->BodyHandle(), PropulsionForce, false);
    }
}
} // namespace ECSEngine