#include "stdafx.h"

#include "Joints.h"

#include "PhysicsEngine.h"
#include "RigidBody.h"

namespace ECSEngine
{
namespace Physics
{

void JointBase::Initialize(const PhysicsBodyHandle& BodyA, const PhysicsBodyHandle& BodyB)
{
    FBodyA = BodyA;
    FBodyB = BodyB;
    bIsland = false;
}

void JointBase::Destroy()
{
    FBodyA = PhysicsBodyHandle{ };
    FBodyB = PhysicsBodyHandle{ };
}

void DistanceJoint::Initialize(const PhysicsBodyHandle& BodyA, const PhysicsBodyHandle& BodyB, const DistanceJointConfig& Config)
{
    JointBase::Initialize(BodyA, BodyB);
    FConfig = Config;
    FImpulse = 0.f;
}

void BallJoint::Initialize(const PhysicsBodyHandle& BodyA, const PhysicsBodyHandle& BodyB, const BallJointConfig& Config)
{
    JointBase::Initialize(BodyA, BodyB);
    FConfig = Config;

    RigidBody* bodyA = PhysicsEngine::Instance().GetRigidBody(BodyA);
    RigidBody* bodyB = PhysicsEngine::Instance().GetRigidBody(BodyB);
    AssertRelease(bodyA && bodyB);

    FPointImpulse = vec3();

    FAxisALocal = Normalize(Config.FConeAxisALocal);
    {
        const mat4 RA = mat4(bodyA->FOrientation);
        const mat4 RB = mat4(bodyB->FOrientation);
        const vec3 axisW = Normalize((RA * vec4::MakeHomogeneousDirectionVec4(FAxisALocal)).xyz());
        FAxisBLocal = Normalize((Transpose(RB) * vec4::MakeHomogeneousDirectionVec4(axisW)).xyz());
    }
    FConeLimitImpulse = 0.f;

    FEnableMotor = FConfig.FEnableMotor;
    FMotorVelocityLocal = FConfig.FMotorVelocityLocal;
    FMaxMotorTorque = FConfig.FMaxMotorTorque;

    FUseTargetOrientation = FConfig.FUseTargetOrientation;
    FTargetEulerAngles = FConfig.FTargetEulerAngles;
    FMotorStiffness = FConfig.FMotorStiffness;

    FMotorImpulse = vec3{ };
}

void HingeJoint::Initialize(const PhysicsBodyHandle& BodyA, const PhysicsBodyHandle& BodyB, const HingeJointConfig& Config)
{
    JointBase::Initialize(BodyA, BodyB);

    FConfig = Config;

    RigidBody* bodyA = PhysicsEngine::Instance().GetRigidBody(BodyA);
    RigidBody* bodyB = PhysicsEngine::Instance().GetRigidBody(BodyB);
    AssertRelease(bodyA && bodyB);

    const mat4 RA = mat4(bodyA->FOrientation);
    const mat4 RB = mat4(bodyB->FOrientation);

    FAxisALocal = Normalize(Config.FAxisALocal);
    const vec3 axisW = Normalize((RA * vec4::MakeHomogeneousDirectionVec4(FAxisALocal)).xyz());

    const mat4 RBinv = Transpose(RB);
    FAxisBLocal = Normalize((RBinv * vec4::MakeHomogeneousDirectionVec4(axisW)).xyz());

    vec3 t = (abs(axisW.x) < 0.9f) ? vec3(1, 0, 0) : vec3(0, 1, 0);

    const vec3 refW = Normalize(Cross(axisW, t));
    FRefALocal = Normalize((Transpose(RA) * vec4::MakeHomogeneousDirectionVec4(refW)).xyz());
    FRefBLocal = Normalize((RBinv * vec4::MakeHomogeneousDirectionVec4(refW)).xyz());

    FReferenceAngle = 0.f;
    FPrevAngle = 0.f;

    FPointImpulse = vec3();
    FAxisImpulse[0] = FAxisImpulse[1] = 0.f;
    FLimitImpulse = 0.f;
    FLimitState = EHingeLimitState::Inactive;

    FEnableMotor = FConfig.FEnableMotor;
    FMotorSpeed = FConfig.FMotorSpeed;
    FMaxMotorTorque = FConfig.FMaxMotorTorque;

    FUseTargetAngle = FConfig.FUseTargetAngle;
    FTargetAngle = FConfig.FTargetAngle;
    FMotorStiffness = FConfig.FMotorStiffness;

    FMotorImpulse = 0.f;
}

} // namespace Physics
} // namespace ECSEngine
