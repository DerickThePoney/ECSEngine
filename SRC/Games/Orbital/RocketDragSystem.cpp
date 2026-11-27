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
namespace RocketDragHelpers
{
bool ComputeDynamicPressureAndVHat(const float rho, const vec3& rocketVel, const vec3& planetVel, float& outQ, vec3& outVHat)
{
    // Compute relative speed
    const vec3 vRel = rocketVel - planetVel;
    const float speedSq = LengthSq(vRel);

    // below a speed of 0.5, there's no effect of atmosphere.
    if (speedSq < 0.25f)
        return false;

    outQ = 0.5f * rho * speedSq; // dynamic pressure

    const float speed = sqrt(speedSq);
    outVHat = vRel / speed;
    return true;
}

void ComputeNoseOrientation(ModuleAccessor<RigidbodyModule>& rigidbodyAccessor, const EntityId& RootToUse, vec3& OutNoseDirection)
{
    const RigidbodyModule* rootBody = rigidbodyAccessor[RootToUse];
    AssertRelease(rootBody != nullptr);

    quat bodyOrientation;
    bool bBodyOrientation = Physics::GetBodyOrientation(rootBody->BodyHandle(), bodyOrientation);
    AssertRelease(bBodyOrientation);

    // get the nose direction
    OutNoseDirection = ((mat4)bodyOrientation).Column(1).xyz();
}

void ComputeDragAccelerationAndWeathercockTorque(const RocketManagerModule& rocketManager,
      const float q,
      const vec3& Nose,
      const vec3& vHat,
      const float mass,
      vec3& OutDrag,
      vec3& OutWeathercock)
{
    // computing actual drag
    const float Alignment = Min(1.f, fabs(Dot(Nose, vHat)));
    const float presentedArea = rocketManager.AxialDragArea() * Alignment + rocketManager.BroadsideDragArea() * sqrt(1.f - Alignment * Alignment);

    const vec3 DragForce = -q * presentedArea * vHat;

    // Computing drag accelleration and apply
    OutDrag = DragForce / mass;

    // Compute weathercock
    OutWeathercock = rocketManager.Weathercock() * q * presentedArea * Cross(Nose, vHat);
}

bool ComputeDragForActiveBodies(ModuleAccessor<RigidbodyModule>& rigidbodyAccessor,
      const RocketManagerModule& rocketManager,
      const PlanetAtmosphereModule* atmosphereModule,
      const vec3& planetPos,
      const vec3& planetVel,
      const float planetRadius)
{
    // Get rocket COM and Vel
    vec3 rocketCom, rocketVel;
    bool bRocketComAndVel = rocketManager.ComputeActiveCenterOfMass(rigidbodyAccessor, rocketCom, &rocketVel);
    AssertRelease(bRocketComAndVel);

    // Compute Altitude and atmospheric pressure
    const float Altitude = Max(0.f, Length(rocketCom - planetPos) - planetRadius);

    const float rho = atmosphereModule->DensityAt(Altitude);
    if (rho < 1e-6f)
        return false;

    float q = 0.f;
    vec3 vHat;
    if (!ComputeDynamicPressureAndVHat(rho, rocketVel, planetVel, q, vHat))
        return false;

    const EntityId RootId = rocketManager.RootId();
    vec3 Nose;
    ComputeNoseOrientation(rigidbodyAccessor, RootId, Nose);

    float mass = 0.f;
    if (!rocketManager.ComputeActiveTotalMass(rigidbodyAccessor, mass))
        return false;

    vec3 DragAcceleration, WeathercockTorque;
    ComputeDragAccelerationAndWeathercockTorque(rocketManager, q, Nose, vHat, mass, DragAcceleration, WeathercockTorque);

    // Apply Drag and Weathercock
    rocketManager.ApplyForceToAllActiveBodies(rigidbodyAccessor, DragAcceleration, true);

    const RigidbodyModule* rootBody = rigidbodyAccessor[RootId];
    AssertRelease(rootBody != nullptr);
    Physics::AddTorqueToBody(rootBody->BodyHandle(), WeathercockTorque);

    return true;
}

bool ComputeDragForDebris(ModuleAccessor<RigidbodyModule>& rigidbodyAccessor,
      const RocketManagerModule& rocketManager,
      const PlanetAtmosphereModule* atmosphereModule,
      const vec3& planetPos,
      const vec3& planetVel,
      const float planetRadius)
{
    forrange(i, 0, rocketManager.DebrisCount())
    {
        // Get rocket COM and Vel
        vec3 debrisCom, DebrisVel;
        bool bRocketComAndVel = rocketManager.ComputeCenterOfMassInDebrisIndex(rigidbodyAccessor, i, debrisCom, &DebrisVel);
        AssertRelease(bRocketComAndVel);

        // Compute Altitude and atmospheric pressure
        const float Altitude = Max(0.f, Length(debrisCom - planetPos) - planetRadius);

        const float rho = atmosphereModule->DensityAt(Altitude);
        if (rho < 1e-6f)
            continue;

        float q = 0.f;
        vec3 vHat;
        if (!ComputeDynamicPressureAndVHat(rho, DebrisVel, planetVel, q, vHat))
            continue;

        const EntityId RootId = rocketManager.RootIdForDebrisIndex(i);
        vec3 Nose;
        ComputeNoseOrientation(rigidbodyAccessor, RootId, Nose);

        float mass = 0.f;
        if (!rocketManager.ComputeTotalMassInDebrisIndex(rigidbodyAccessor, i, mass))
            continue;

        vec3 DragAcceleration, WeathercockTorque;
        ComputeDragAccelerationAndWeathercockTorque(rocketManager, q, Nose, vHat, mass, DragAcceleration, WeathercockTorque);

        // Apply Drag and Weathercock
        rocketManager.ApplyForceToAllBodiesInDebrisIndex(rigidbodyAccessor, i, DragAcceleration, true);

        const RigidbodyModule* rootBody = rigidbodyAccessor[RootId];
        AssertRelease(rootBody != nullptr);
        Physics::AddTorqueToBody(rootBody->BodyHandle(), WeathercockTorque);
    }

    return true;
}
} // namespace RocketDragHelpers

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

        const RigidbodyModule* planetBody = rigidbodyAccessor[parentId];
        AssertRelease(planetBody != nullptr);

        vec3 planetPos, planetVel;
        bool bPlanetPos = Physics::GetBodyCenterOfMass(planetBody->BodyHandle(), planetPos);
        AssertRelease(bPlanetPos);

        bool bPlanetVel = Physics::GetBodyVelocity(planetBody->BodyHandle(), planetVel);
        AssertRelease(bPlanetVel);

        const PlanetGravityModule* gravityModule = planetGravityAccessor[parentId];
        AssertRelease(gravityModule != nullptr);

        RocketDragHelpers::ComputeDragForActiveBodies(rigidbodyAccessor, rocketManager, atmosphereModule, planetPos, planetVel, gravityModule->PlanetRadius());
        RocketDragHelpers::ComputeDragForDebris(rigidbodyAccessor, rocketManager, atmosphereModule, planetPos, planetVel, gravityModule->PlanetRadius());
    }
}
} // namespace ECSEngine
