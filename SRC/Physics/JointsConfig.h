#pragma once

namespace ECSEngine
{
namespace Physics
{
struct JointConfig
{
    vec3 FAnchorALocal;
    vec3 FAnchorBLocal;

    void DrawInEditor();

    SERIALIZE()
    {
        PROPERTYFIELD(AnchorALocal, vec3());
        PROPERTYFIELD(AnchorBLocal, vec3());
    }
};

struct DistanceJointConfig
{
    JointConfig FCommon;
    float FLength = 0.f;

    void DrawInEditor();

    SERIALIZE()
    {
        PROPERTYFIELD(Common, JointConfig{ });
        PROPERTYFIELD(Length, 0.1f);
    }
};

} // namespace Physics
} // namespace ECSEngine