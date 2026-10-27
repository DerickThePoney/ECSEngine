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

struct BallJointConfig
{
    JointConfig FCommon;

    void DrawInEditor();

    SERIALIZE() { PROPERTYFIELD(Common, JointConfig{ }); }
};

struct HingeJointConfig
{
    JointConfig FCommon;

    void DrawInEditor();

    vec3 FAxisALocal; // hinge axis in A (normalize at create)
    bool FEnableLimit = false;
    float FLowerAngle = 0.f; // radians, relative to bind
    float FUpperAngle = 0.f;

    SERIALIZE()
    {
        PROPERTYFIELD(Common, JointConfig{ });
        PROPERTYFIELD(AxisALocal, vec3{ });
        PROPERTYFIELD(EnableLimit, false);
        PROPERTYFIELD(LowerAngle, 0.f);
        PROPERTYFIELD(UpperAngle, 0.f);
    }
};

} // namespace Physics
} // namespace ECSEngine