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
};
} // namespace Physics
} // namespace ECSEngine