#pragma once
#include "Math/VectorTypes.h"

namespace ECSEngine
{
namespace Physics
{
struct RigidBody
{
    vec3 FPosition;
    vec3 FVelocity;
    vec3 FAcceleration;

    float FMass = 0.f;

    float FGravityScale = 1.f;

    float FLinearDamping = 0.1f;
    float FAngularDamping = 0.01f;
};
} // namespace Physics
} // namespace ECSEngine