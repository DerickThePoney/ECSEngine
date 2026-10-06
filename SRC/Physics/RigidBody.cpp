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

void RigidBody::AddTorque(const vec3& Torque)
{
    FTorque += Torque;
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

void RigidBody::AddRotationImpulse(const vec3& Impulse, bool bTreatAsRotationVelocityChange)
{
    if (bTreatAsRotationVelocityChange)
    {
        FRotationVelocity += Impulse;
    }
    else
    {
        FRotationVelocity += FInverseInertiaTensorWorld * Impulse;
    }
}

mat4 RigidBody::GetTransform() const
{
    return Translation(FPosition) * (mat4)FOrientation;
}

void RigidBody::SetAwake(bool bValue)
{
    if (bValue)
    {
        if (!IsAwake())
        {
            FFlags.SetBit(ERigidBodyFlag::RB_AWAKE, true);
            FSleepingTimer = 0.f;
        }
    }
    else
    {
        FSleepingTimer = 0.f;
        FFlags.SetBit(ERigidBodyFlag::RB_AWAKE, false);
    }
}

bool RigidBody::IsAwake() const
{
    return FFlags.GetValue(ERigidBodyFlag::RB_AWAKE);
}

AABB3f RigidBody::ComputeAABB(const mat4& parTransform) const
{
    AABB3f result;
    foreachitemconst(shape, FCollisionShapes)
    {
        result = AABB3f::Union(shape.ComputeAABB(parTransform), result);
    }
    return result;
}

void RigidBody::UpdateCoMWorld()
{
    FCenterOfMassWorld = (GetTransform() * vec4::MakeHomogeneousPositionVec4(FCenterOfMassLocal)).xyz();
}

} // namespace Physics
} // namespace ECSEngine
