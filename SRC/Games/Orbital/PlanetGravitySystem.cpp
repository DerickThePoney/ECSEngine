#include "stdafx.h"

#include "PlanetGravitySystem.h"

#include "ECSCore/ModuleAccessor.h"
#include "ECSCore/WorldIds.h"
#include "ECSGameplay_Common/RigidbodyModule.h"
#include "Physics/PhysicsAPI.h"
#include "PlanetGravityModule.h"
#include "RocketManagerModule.h"

namespace ECSEngine
{
void PlanetGravitySystem::VirtualInit()
{
    ModuleSystem::VirtualInit();

    RegisterDependency<RigidbodyModule>(EEntityWorlds::STANDARD);
    RegisterDependency<PlanetGravityModule>(EEntityWorlds::STANDARD);
    RegisterDependency<RocketManagerModule>(EEntityWorlds::STANDARD);
}

void PlanetGravitySystem::VirtualUpdate()
{
    ModuleSystem::VirtualUpdate();

    ModuleAccessor<RocketManagerModule> rocketManagerAccessor(EEntityWorlds::STANDARD);
    ModuleAccessor<RigidbodyModule> rigidbodyAccessor(EEntityWorlds::STANDARD);
    ModuleAccessor<PlanetGravityModule> planetGravityAccessor(EEntityWorlds::STANDARD);

    foreachitem(rocket, rocketManagerAccessor)
    {
        const RigidbodyModule* const bodyRB = rigidbodyAccessor[rocket.BodyId()];
        AssertRelease(bodyRB != nullptr);
        const RigidbodyModule* const nozzleRB = rigidbodyAccessor[rocket.NozzleId()];
        AssertRelease(nozzleRB != nullptr);

        vec3 vesselPosition;
        const bool hasVesselCenterOfMass = rocket.ComputeCenterOfMass(rigidbodyAccessor, vesselPosition);
        AssertRelease(hasVesselCenterOfMass);
        if (!hasVesselCenterOfMass)
            continue;

        vec3 gravityAcceleration{ 0.f };

        foreachitemconst(planetGravity, planetGravityAccessor)
        {
            const RigidbodyModule* const planetRB = rigidbodyAccessor[planetGravity.UnitId()];
            AssertRelease(planetRB != nullptr);

            vec3 planetPosition;
            const bool hasPlanetCenter = Physics::GetBodyCenterOfMass(planetRB->BodyHandle(), planetPosition);
            AssertRelease(hasPlanetCenter);
            if (!hasPlanetCenter)
                continue;

            vec3 r = vesselPosition - planetPosition;
            const float lr = Length(r);
            const float lr_3 = lr * lr * lr;
            gravityAcceleration -= planetGravity.PlanetGravityConstant() * r / lr_3;
        }

        // Hackos
        rocket.SetLastTurnGravity(gravityAcceleration);

        Physics::AddForceToBody(bodyRB->BodyHandle(), gravityAcceleration, true);
        Physics::AddForceToBody(nozzleRB->BodyHandle(), gravityAcceleration, true);
    }
}
} // namespace ECSEngine
