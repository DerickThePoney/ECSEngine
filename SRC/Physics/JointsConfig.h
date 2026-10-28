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

    bool FEnableConeLimit = false;
    vec3 FConeAxisALocal = vec3(0.f, 1.f, 0.f); // swing reference in A
    float FMaxConeAngle = 1.57079632679f; // ~Pi/2 radians

    bool FEnableMotor = false;
    vec3 FMotorVelocityLocal;
    float FMaxMotorTorque = 0.f;

    bool FUseTargetOrientation = false;
    vec3 FTargetEulerAngles;
    float FMotorStiffness = 0.f;

    void DrawInEditor();

    SERIALIZE()
    {
        PROPERTYFIELD(Common, JointConfig{ });
        PROPERTYFIELD(EnableConeLimit, false);
        PROPERTYFIELD(ConeAxisALocal, vec3(0.f, 1.f, 0.f));
        PROPERTYFIELD(MaxConeAngle, 1.57079632679f);
        PROPERTYFIELD(EnableMotor, false);
        PROPERTYFIELD(MotorVelocityLocal, vec3{ });
        PROPERTYFIELD(MaxMotorTorque, 0.f);
        PROPERTYFIELD(UseTargetOrientation, false);
        PROPERTYFIELD(TargetEulerAngles, vec3{ });
        PROPERTYFIELD(MotorStiffness, 0.f);
    }
};

struct HingeJointConfig
{
    JointConfig FCommon;

    void DrawInEditor();

    vec3 FAxisALocal; // hinge axis in A (normalize at create)
    bool FEnableLimit = false;
    float FLowerAngle = 0.f; // radians, relative to bind
    float FUpperAngle = 0.f;

    bool FEnableMotor = false;
    float FMotorSpeed = 0.f;
    float FMaxMotorTorque = 0.f;

    bool FUseTargetAngle = false;
    float FTargetAngle = 0.f;
    float FMotorStiffness = 0.f;

    SERIALIZE()
    {
        PROPERTYFIELD(Common, JointConfig{ });
        PROPERTYFIELD(AxisALocal, vec3{ });
        PROPERTYFIELD(EnableLimit, false);
        PROPERTYFIELD(LowerAngle, 0.f);
        PROPERTYFIELD(UpperAngle, 0.f);
        PROPERTYFIELD(EnableMotor, false);
        PROPERTYFIELD(MotorSpeed, 0.f);
        PROPERTYFIELD(MaxMotorTorque, 0.f);
        PROPERTYFIELD(UseTargetAngle, false);
        PROPERTYFIELD(TargetAngle, 0.f);
        PROPERTYFIELD(MotorStiffness, 0.f);
    }
};

} // namespace Physics
} // namespace ECSEngine