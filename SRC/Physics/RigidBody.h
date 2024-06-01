#pragma once
#include "Math/VectorTypes.h"

namespace ECSEngine
{
namespace Physics
{
struct RigidBody
{
    DECLARE_POOL_ALLOCATED(RigidBody);

public:
    // positional stuff
    vec3 FPosition;
    vec3 FVelocity;
    vec3 FAccelerationDueToForces;

    // angular stuff
    quat FOrientation;
    vec3 FRotationVelocity;
    vec3 FTorque;

    // COM
    vec3 FCenterOfMassLocal;
    /*vec3 FCenterOfMassWorld;*/

    // Mass and inertia
    mat3 FInertiaTensor;
    mat3 FInverseInitiaTensor;
    float FMass = 0.f;
    float FInvMass = 1.f;

    float FGravityScale = 1.f;

    float FLinearDamping = 0.1f;
    float FAngularDamping = 0.01f;
};
} // namespace Physics
} // namespace ECSEngine