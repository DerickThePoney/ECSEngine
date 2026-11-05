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
quat OrientationFromNose(const vec3& noseDirection, const vec3& radialOut, const vec3& orbitNormal)
{
    const vec3 nose = Normalize(noseDirection);

    vec3 hint = radialOut;
    vec3 side = Cross(nose, hint);
    if (LengthSq(side) < 0.02f * LengthSq(hint))
    {
        hint = orbitNormal;
        side = Cross(nose, hint);
    }
    if (LengthSq(side) < 1.e-8f)
    {
        const vec3 fallback = (fabsf(nose.x) < 0.9f) ? vec3(1.f, 0.f, 0.f) : vec3(0.f, 0.f, 1.f);
        side = Cross(nose, fallback);
    }

    side = Normalize(side);
    const vec3 dorsal = Normalize(Cross(side, nose));

    const mat4 basis(vec4::MakeHomogeneousDirectionVec4(side), vec4::MakeHomogeneousDirectionVec4(nose), vec4::MakeHomogeneousDirectionVec4(dorsal), vec4(0.f, 0.f, 0.f, 1.f));
    return Quaternionf::FromMat4(basis);
}

void ComputeSASTarget(RocketManagerModule& rocketManager, ModuleAccessor<RigidbodyModule>& rigidbodyAccessor, ModuleAccessor<PlanetGravityModule>& planetAccessor)
{
    if (rocketManager.SASMode() == ESASMode::STABILITY)
    {
        if (rocketManager.HasSASTarget())
            return;

        const RigidbodyModule* bodyRigidbody = rigidbodyAccessor[rocketManager.BodyId()];
        quat target;
        Physics::GetBodyOrientation(bodyRigidbody->BodyHandle(), target);
        rocketManager.SetSASTarget(target);
        return;
    }

    const RigidbodyModule* bodyRigidbody = rigidbodyAccessor[rocketManager.BodyId()];
    vec3 rocketPosition;
    vec3 rocketVelocity;
    Physics::GetBodyPosition(bodyRigidbody->BodyHandle(), rocketPosition);
    Physics::GetBodyVelocity(bodyRigidbody->BodyHandle(), rocketVelocity);

    if (planetAccessor.size() != 1)
        return;

    const PlanetGravityModule& planet = *planetAccessor.begin();
    const RigidbodyModule* planetBody = rigidbodyAccessor[planet.UnitId()];
    AssertRelease(planetBody != nullptr);

    vec3 planetPosition;
    Physics::GetBodyCenterOfMass(planetBody->BodyHandle(), planetPosition);

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

    rocketManager.SetSASTarget(OrientationFromNose(TargetDirection, radialOut, orbitNormal));
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
        const RigidbodyModule* bodyRigidbody = rigidbodyAccessor[rocketManager.BodyId()];
        quat qCurrent;
        Physics::GetBodyOrientation(bodyRigidbody->BodyHandle(), qCurrent);
        quat qError = rocketManager.SASTarget() * Conjugate(qCurrent);

        if (qError.w < 0)
        {
            qError = Invert(qError);
        }

        vec3 Axis = vec3(qError.x, qError.y, qError.z);
        float Angle = Atan2(Length(Axis), qError.w) * 2;

        ADJUSTABLE_DEBUG_PARAMETER_SINGLE(stiffness, 80.f, "Stiffness", "SAS", 15.f, 500.f);
        ADJUSTABLE_DEBUG_PARAMETER_SINGLE(damping, 80.f, "Damping", "SAS", 15.f, 500.f);
        ADJUSTABLE_DEBUG_PARAMETER_SINGLE(maxTorque, 300.f, "Max torque", "SAS", 15.f, 5000.f);

        vec3 Omega;
        Physics::GetBodyRotationVelocity(bodyRigidbody->BodyHandle(), Omega);
        vec3 torque = -damping * Omega;
        if (LengthSq(Axis) > 1.e-8f)
            torque += stiffness * Angle * Normalize(Axis);
        if (Length(torque) > maxTorque)
            torque = Normalize(torque) * maxTorque;

        Physics::AddTorqueToBody(bodyRigidbody->BodyHandle(), torque);
    }
}
} // namespace ECSEngine