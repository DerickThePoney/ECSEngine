#pragma once
#include "Math/VectorTypes.h"

namespace ECSEngine
{
struct RigidBody
{
    vec3 FPosition;
    vec3 FSpeed;
    vec3 FAcceleration;
};
} // namespace ECSEngine