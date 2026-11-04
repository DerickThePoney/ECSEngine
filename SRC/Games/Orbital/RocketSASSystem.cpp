#include "stdafx.h"

#include "RocketSASSystem.h"

#include "ECSCore/ModuleAccessor.h"
#include "ECSCore/WorldIds.h"
#include "ECSGameplay_Common/RigidbodyModule.h"
#include "Physics/PhysicsAPI.h"
#include "RocketManagerModule.h"

namespace ECSEngine
{
namespace SASHelpers
{
void ComputeSASTarget(RocketManagerModule& rocketManager, ModuleAccessor<RigidbodyModule>& rigidbodyAccessor)
{
    vec3 TargetDirection;
    switch (rocketManager.SASMode())
    {
    case ESASMode::STABILITY:
    {
        if (rocketManager.HasSASTarget())
        {
            return;
        }

        const RigidbodyModule* bodyRigidbody = rigidbodyAccessor[rocketManager.BodyId()];
        quat target;
        Physics::GetBodyOrientation(bodyRigidbody->BodyHandle(), target);
        rocketManager.SetSASTarget(target);
        return;
        break;
    }
    case ESASMode::PROGRADE:
    {
        const RigidbodyModule* bodyRigidbody = rigidbodyAccessor[rocketManager.BodyId()];
        vec3 rocketVelocity;
        Physics::GetBodyVelocity(bodyRigidbody->BodyHandle(), rocketVelocity);
        TargetDirection = Normalize(rocketVelocity);
        break;
    }
    case ESASMode::RETROGRADE:
    {
        const RigidbodyModule* bodyRigidbody = rigidbodyAccessor[rocketManager.BodyId()];
        vec3 rocketVelocity;
        Physics::GetBodyVelocity(bodyRigidbody->BodyHandle(), rocketVelocity);
        TargetDirection = Invert(Normalize(rocketVelocity));
        break;
    }
    }

    // Convert to quaternion
}
} // namespace SASHelpers

RocketSASSystem::RocketSASSystem()
{
    RegisterDependency<RocketManagerModule>(EEntityWorlds::STANDARD);
    RegisterDependency<RigidbodyModule>(EEntityWorlds::STANDARD);
}

void RocketSASSystem::VirtualUpdate()
{
    ModuleSystem::VirtualUpdate();

    ModuleAccessor<RocketManagerModule> rocketManagerAccessor(EEntityWorlds::STANDARD);
    ModuleAccessor<RigidbodyModule> rigidbodyAccessor(EEntityWorlds::STANDARD);

    AssertRelease(rocketManagerAccessor.size() <= 1); // Pour l'instant on en prend qu'un !!

    foreachitem(rocketManager, rocketManagerAccessor)
    {
        if (!rocketManager.SASActivated())
            continue;

        if (rocketManager.SASMode() == ESASMode::STABILITY && rocketManager.IsSteering())
        {
            rocketManager.ClearSASTarget();
        }

        // Build Target
        if (!rocketManager.HasSASTarget() && !rocketManager.IsSteering())
        {
            SASHelpers::ComputeSASTarget(rocketManager, rigidbodyAccessor);
        }

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

        constexpr float stiffness = 40.0f;
        constexpr float damping = 80.f;
        constexpr float maxTorque = 150.f;

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