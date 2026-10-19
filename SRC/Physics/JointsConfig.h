#pragma once

namespace ECSEngine
{
namespace Physics
{
struct JointConfig
{
    vec3 FAnchorALocal;
    vec3 FAnchorBLocal;
};

struct DistanceJointConfig
{
    JointConfig Common;
    float FLength = 0.f;
};

} // namespace Physics
} // namespace ECSEngine