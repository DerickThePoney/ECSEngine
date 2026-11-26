#include "stdafx.h"

#include "RocketSASSystem.h"

#include "ECSCore/AdjustableDebugParameters.h"
#include "ECSCore/ModuleAccessor.h"
#include "ECSCore/WorldIds.h"
#include "ECSGameplay_Common/RigidbodyModule.h"
#include "Physics/PhysicsAPI.h"
#include "PlanetGravityModule.h"
#include "RocketManagerModule.h"

namespace ECSEngine
{
namespace SASHelpers
{
// Point the body +Y axis at noseDirection with the smallest rotation from `reference`
// (preserves roll / avoids the scenic SO(3) path when only pointing matters).
quat OrientationAlignNose(const vec3& noseDirection, const quat& reference)
{
    const vec3 desired = Normalize(noseDirection);
    const vec3 current = Normalize((mat4(reference) * vec4(0.f, 1.f, 0.f, 0.f)).xyz());
    const float d = Clamp(Dot(current, desired), -1.f, 1.f);
    if (d > 0.999999f)
        return reference;

    vec3 axis;
    float angle;
    if (d < -0.999999f)
    {
        axis = Normalize((mat4(reference) * vec4(1.f, 0.f, 0.f, 0.f)).xyz());
        angle = Pi();
    }
    else
    {
        axis = Normalize(Cross(current, desired));
        angle = acosf(d);
    }
    return quat(axis, angle) * reference;
}

void ComputeSASTarget(RocketManagerModule& rocketManager, ModuleAccessor<RigidbodyModule>& rigidbodyAccessor, ModuleAccessor<PlanetGravityModule>& planetAccessor)
{
    if (rocketManager.SASMode() == ESASMode::STABILITY)
    {
        if (rocketManager.HasSASTarget())
            return;

        const RigidbodyModule* rootBody = rigidbodyAccessor[rocketManager.RootId()];
        quat target;
        Physics::GetBodyOrientation(rootBody->BodyHandle(), target);
        rocketManager.SetSASTarget(target);
        return;
    }

    const RigidbodyModule* rootBody = rigidbodyAccessor[rocketManager.RootId()];
    vec3 rocketPosition;
    vec3 rocketVelocity;
    rocketManager.ComputeCenterOfMass(rigidbodyAccessor, rocketPosition, &rocketVelocity);

    const PlanetGravityModule* planet = planetAccessor[rocketManager.CurrentGravityTarget()];
    AssertRelease(planet != nullptr);
    const RigidbodyModule* planetBody = rigidbodyAccessor[planet->UnitId()];
    AssertRelease(planetBody != nullptr);

    vec3 planetPosition;
    vec3 planetVelocity;
    Physics::GetBodyCenterOfMass(planetBody->BodyHandle(), planetPosition);
    Physics::GetBodyVelocity(planetBody->BodyHandle(), planetVelocity);
    rocketVelocity -= planetVelocity;

    const vec3 radialOut = rocketPosition - planetPosition;
    const vec3 orbitNormal = Cross(radialOut, rocketVelocity);
    if (LengthSq(radialOut) < 1.e-4f)
        return;

    vec3 TargetDirection;
    switch (rocketManager.SASMode())
    {
    case ESASMode::PROGRADE:
        if (LengthSq(rocketVelocity) < 0.25f)
            return;
        TargetDirection = rocketVelocity;
        break;
    case ESASMode::RETROGRADE:
        if (LengthSq(rocketVelocity) < 0.25f)
            return;
        TargetDirection = Invert(rocketVelocity);
        break;
    case ESASMode::RADIALOUT:
        TargetDirection = radialOut;
        break;
    case ESASMode::RADIALIN:
        TargetDirection = Invert(radialOut);
        break;
    case ESASMode::NORMAL:
        if (LengthSq(orbitNormal) < 1.e-4f)
            return;
        TargetDirection = orbitNormal;
        break;
    case ESASMode::ANTINORMAL:
        if (LengthSq(orbitNormal) < 1.e-4f)
            return;
        TargetDirection = Invert(orbitNormal);
        break;
    default:
        return;
    }

    quat reference;
    if (rocketManager.HasSASTarget())
        reference = rocketManager.SASTarget();
    else
        Physics::GetBodyOrientation(rootBody->BodyHandle(), reference);

    rocketManager.SetSASTarget(OrientationAlignNose(TargetDirection, reference));
}
} // namespace SASHelpers

RocketSASSystem::RocketSASSystem()
{
    RegisterDependency<RocketManagerModule>(EEntityWorlds::STANDARD);
    RegisterDependency<RigidbodyModule>(EEntityWorlds::STANDARD);
    RegisterDependency<PlanetGravityModule>(EEntityWorlds::STANDARD);
}

void RocketSASSystem::VirtualUpdate()
{
    ModuleSystem::VirtualUpdate();

    ModuleAccessor<RocketManagerModule> rocketManagerAccessor(EEntityWorlds::STANDARD);
    ModuleAccessor<RigidbodyModule> rigidbodyAccessor(EEntityWorlds::STANDARD);
    ModuleAccessor<PlanetGravityModule> planetAccessor(EEntityWorlds::STANDARD);

    AssertRelease(rocketManagerAccessor.size() <= 1); // Pour l'instant on en prend qu'un !!

    foreachitem(rocketManager, rocketManagerAccessor)
    {
        if (!rocketManager.SASActivated())
            continue;

        if (rocketManager.IsSteering())
            rocketManager.ClearSASTarget();
        else
            SASHelpers::ComputeSASTarget(rocketManager, rigidbodyAccessor, planetAccessor);

        if (!rocketManager.HasSASTarget())
            continue;

        // Steer towards target
        const RigidbodyModule* rootBody = rigidbodyAccessor[rocketManager.RootId()];
        quat qCurrent;
        Physics::GetBodyOrientation(rootBody->BodyHandle(), qCurrent);
        quat qError = rocketManager.SASTarget() * Conjugate(qCurrent);

        if (qError.w < 0)
        {
            qError = Invert(qError);
        }

        vec3 Axis = vec3(qError.x, qError.y, qError.z);
        float Angle = Atan2(Length(Axis), qError.w) * 2;

        // Millington arrive-align: desiredω = min(maxAngSpeed, Angle/timeToTarget).
        // Near target this matches old PD (stiffness=gain/timeToTarget, damping=gain);
        // far away it cruises at maxAngSpeed instead of seeking harder with angle.
        ADJUSTABLE_DEBUG_PARAMETER_SINGLE(maxAngSpeed, 2.f, "Max ang speed", "SAS", 0.1f, 10.f);
        ADJUSTABLE_DEBUG_PARAMETER_SINGLE(timeToTarget, 1.f, "Arrive time to target", "SAS", 0.1f, 5.f);
        ADJUSTABLE_DEBUG_PARAMETER_SINGLE(arriveGain, 80.f, "Arrive gain", "SAS", 15.f, 500.f);
        ADJUSTABLE_DEBUG_PARAMETER_SINGLE(targetRadius, 0.02f, "Arrive target radius", "SAS", 0.f, 0.2f);
        ADJUSTABLE_DEBUG_PARAMETER_SINGLE(maxTorque, 300.f, "Max torque", "SAS", 15.f, 5000.f);

        vec3 Omega;
        Physics::GetBodyRotationVelocity(rootBody->BodyHandle(), Omega);

        vec3 desiredOmega(0.f);
        const float ttt = Max(timeToTarget, 1.e-3f);
        if (LengthSq(Axis) > 1.e-8f && Angle > targetRadius)
            desiredOmega = Normalize(Axis) * Min(maxAngSpeed, Angle / ttt);

        vec3 torque = arriveGain * (desiredOmega - Omega);
        if (Length(torque) > maxTorque)
            torque = Normalize(torque) * maxTorque;

        Physics::AddTorqueToBody(rootBody->BodyHandle(), torque);
    }
}
} // namespace ECSEngine