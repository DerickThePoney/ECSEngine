#pragma once
#include "JointsConfig.h"
#include "Math/VectorTypes.h"
#include "PhysicsBodyHandle.h"
#include "PhysicsJointHandle.h"

namespace ECSEngine
{
namespace Physics
{
struct JointBase
{
    void Initialize(const PhysicsBodyHandle& BodyA, const PhysicsBodyHandle& BodyB);
    virtual void Destroy();

    PhysicsBodyHandle FBodyA;
    PhysicsBodyHandle FBodyB;

    bool bIsland = false;
};

struct DistanceJoint : public JointBase
{
    DistanceJointConfig FConfig;
    float FImpulse = 0.f;

    void Initialize(const PhysicsBodyHandle& BodyA, const PhysicsBodyHandle& BodyB, const DistanceJointConfig& Config);
};

struct BallJoint : public JointBase
{
    BallJointConfig FConfig;

    vec3 FPointImpulse;

    vec3 FAxisALocal;
    vec3 FAxisBLocal;
    float FConeLimitImpulse = 0.f;

    bool FEnableMotor = false;
    vec3 FMotorVelocityLocal;
    float FMaxMotorTorque = 0.f;

    bool FUseTargetOrientation = false;
    vec3 FTargetEulerAngles;
    float FMotorStiffness = 0.f;

    vec3 FMotorImpulse;

    void Initialize(const PhysicsBodyHandle& BodyA, const PhysicsBodyHandle& BodyB, const BallJointConfig& Config);
};

enum class EHingeLimitState : u8
{
    Inactive,
    AtLower,
    AtUpper
};

struct HingeJoint : public JointBase
{
    HingeJointConfig FConfig;

    vec3 FAxisALocal;
    vec3 FAxisBLocal;
    vec3 FRefALocal;
    vec3 FRefBLocal;
    float FReferenceAngle = 0.f;
    float FPrevAngle = 0.f;

    vec3 FPointImpulse;
    float FAxisImpulse[2] = { };
    float FLimitImpulse = 0.f;
    EHingeLimitState FLimitState = EHingeLimitState::Inactive;

    bool FEnableMotor = false;
    float FMotorSpeed = 0.f;
    float FMaxMotorTorque = 0.f;

    bool FUseTargetAngle = false;
    float FTargetAngle = 0.f;
    float FMotorStiffness = 0.f;

    float FMotorImpulse = 0.f;

    void Initialize(const PhysicsBodyHandle& BodyA, const PhysicsBodyHandle& BodyB, const HingeJointConfig& Config);
};

} // namespace Physics
} // namespace ECSEngine
