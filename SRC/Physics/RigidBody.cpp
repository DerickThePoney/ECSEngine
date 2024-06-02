#include "stdafx.h"

#include "RigidBody.h"

namespace ECSEngine
{
namespace Physics
{
IMPLEMENT_POOL_ALLOCATED(RigidBody);

void RigidBody::AddForce(const vec3& Force, bool bTreatAsAcceleration)
{
    if (bTreatAsAcceleration)
    {
        FAccelerationDueToForces += Force;
    }
    else
    {
        FAccelerationDueToForces += Force * FInvMass;
    }
}

void RigidBody::AddImpulse(const vec3& Impulse, bool bTreatAsVelocityChange)
{
    if (bTreatAsVelocityChange)
    {
        FVelocity += Impulse;
    }
    else
    {
        FVelocity += Impulse * FInvMass;
    }
}

void RigidBody::AddTorque(const vec3& Torque)
{
    FTorque += Torque;
}

} // namespace Physics
} // namespace ECSEngine
