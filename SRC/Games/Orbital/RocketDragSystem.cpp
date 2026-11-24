#include "stdafx.h"

#include "RocketDragSystem.h"

#include "ECSCore/ModuleAccessor.h"
#include "ECSCore/WorldIds.h"
#include "ECSGameplay_Common/RigidbodyModule.h"
#include "Physics/PhysicsAPI.h"
#include "PlanetAtmosphereModule.h"
#include "PlanetGravityModule.h"
#include "RocketManagerModule.h"

namespace ECSEngine
{
RocketDragSystem::RocketDragSystem()
{
    RegisterDependency<RocketManagerModule>(EEntityWorlds::STANDARD);
    RegisterDependency<RigidbodyModule>(EEntityWorlds::STANDARD);
    RegisterDependency<PlanetAtmosphereModule>(EEntityWorlds::STANDARD);
    RegisterDependency<PlanetGravityModule>(EEntityWorlds::STANDARD);
}

void RocketDragSystem::VirtualUpdate()
{
    ModuleSystem::VirtualUpdate();

    ModuleAccessor<RocketManagerModule> rocketManagerAccessor(EEntityWorlds::STANDARD);
    ModuleAccessor<RigidbodyModule> rigidbodyAccessor(EEntityWorlds::STANDARD);
    ModuleAccessor<PlanetAtmosphereModule> atmosphereAccessor(EEntityWorlds::STANDARD);
    ModuleAccessor<PlanetGravityModule> planetGravityAccessor(EEntityWorlds::STANDARD);

    AssertRelease(rocketManagerAccessor.size() <= 1);
    foreachitemconst(rocketManager, rocketManagerAccessor)
    {
        const EntityId& parentId = rocketManager.CurrentGravityTarget();
        if (!parentId.Valid())
            continue;

        const PlanetAtmosphereModule* atmosphereModule = atmosphereAccessor[parentId];
        if (atmosphereModule == nullptr)
            continue;

        const PlanetGravityModule* gravityModule = planetGravityAccessor[parentId];
        AssertRelease(gravityModule != nullptr);

        // Get rocket COM and Vel
        vec3 rocketCom, rocketVel;
        bool bRocketComAndVel = rocketManager.ComputeCenterOfMass(rigidbodyAccessor, rocketCom, &rocketVel);
        AssertRelease(bRocketComAndVel);

        const RigidbodyModule* planetBody = rigidbodyAccessor[parentId];
        AssertRelease(planetBody != nullptr);

        // Compute Altitude and atmospheric pressure
        vec3 planetPos;
        bool bPlanetPos = Physics::GetBodyCenterOfMass(planetBody->BodyHandle(), planetPos);
        AssertRelease(bPlanetPos);

        const float Altitude = Max(0.f, Length(rocketCom - planetPos) - gravityModule->PlanetRadius());

        const float rho = atmosphereModule->DensityAt(Altitude);
        if (rho < 1e-6f)
            continue;

        // Compute relative speed
        vec3 planetVel;
        bool bPlanetVel = Physics::GetBodyVelocity(planetBody->BodyHandle(), planetVel);
        AssertRelease(bPlanetVel);

        const vec3 vRel = rocketVel - planetVel;
        const float speedSq = LengthSq(vRel);

        // below a speed of 0.5, there's no effect of atmosphere.
        if (speedSq < 0.25f)
            continue;

        const float q = 0.5f * rho * speedSq; // dynamic pressure

        const float speed = sqrt(speedSq);
        const vec3 vHat = vRel / speed;

        const RigidbodyModule* bodyBody = rigidbodyAccessor[rocketManager.BodyId()];
        AssertRelease(bodyBody != nullptr);

        quat bodyOrientation;
        bool bBodyOrientation = Physics::GetBodyOrientation(bodyBody->BodyHandle(), bodyOrientation);
        AssertRelease(bBodyOrientation);

        // get the nose direction
        const vec3 Nose = ((mat4)bodyOrientation).Column(1).xyz();

        // computing actual drag
        const float Alignment = Min(1.f, fabs(Dot(Nose, vHat)));
        const float presentedArea = rocketManager.AxialDragArea() * Alignment + rocketManager.BroadsideDragArea() * sqrt(1.f - Alignment * Alignment);

        const vec3 DragForce = -q * presentedArea * vHat;

        // Computing drag accelleration and apply
        const RigidbodyModule* nozzleBody = rigidbodyAccessor[rocketManager.NozzleId()];
        AssertRelease(nozzleBody != nullptr);

        float bodyMass, nozzleMass;
        bool bBodyMass = Physics::GetBodyMass(bodyBody->BodyHandle(), bodyMass);
        bool bNozzleMass = Physics::GetBodyMass(nozzleBody->BodyHandle(), nozzleMass);
        AssertRelease(bBodyMass && bNozzleMass);

        const float mass = bodyMass + nozzleMass;
        const vec3 DragAcceleration = DragForce / mass;

        Physics::AddForceToBody(bodyBody->BodyHandle(), DragAcceleration, true);
        Physics::AddForceToBody(nozzleBody->BodyHandle(), DragAcceleration, true);

        // Compute weathercock
        const vec3 weathercockTorque = rocketManager.Weathercock() * q * presentedArea * Cross(Nose, vHat);
        Physics::AddTorqueToBody(bodyBody->BodyHandle(), weathercockTorque);
    }
}
} // namespace ECSEngine
