#pragma once
#include "Joints.h"

namespace ECSEngine
{
namespace Physics
{
class Island;
struct RigidBody;

class PhysicsJointsManager
{
public:
    void RemoveBody(RigidBody* Body);

    PhysicsJointHandle CreateDistanceJoint(const PhysicsBodyHandle& BodyA, const PhysicsBodyHandle& BodyB, const DistanceJointConfig& Config);
    PhysicsJointHandle CreateBallJoint(const PhysicsBodyHandle& BodyA, const PhysicsBodyHandle& BodyB, const BallJointConfig& Config);
    PhysicsJointHandle CreateHingeJoint(const PhysicsBodyHandle& BodyA, const PhysicsBodyHandle& BodyB, const HingeJointConfig& Config);

    void DestroyJoint(const PhysicsJointHandle& Handle);

    JointBase* GetJoint(const PhysicsJointHandle& Handle);

    void PreUpdate();

    // Motor
    void SetHingeMotor(const PhysicsJointHandle& JointHandle, bool bEnable, float MotorSpeed, float MaxTorque);
    void SetHingeTargetAngle(const PhysicsJointHandle& JointHandle, bool bEnable, float TargetAngle, float Stiffness, float MaxTorque);
    void SetBallMotor(const PhysicsJointHandle& JointHandle, bool bEnable, const vec3& VelocityInA, float MaxTorque);
    void SetBallTargetEulerAngles(const PhysicsJointHandle& JointHandle, bool bEnable, const vec3& EulerYawRollPitch, float Stiffness, float MaxTorque);

#ifdef PERFORM_SECURITY_CHECKS
    void DebugDrawJoints();
#endif

private:
    struct DistanceJointRecord
    {
        DistanceJoint Joint;
        PhysicsJointHandle Handle;
    };
    std::vector<DistanceJointRecord> FDistanceJoints;

    IdGenerator FDistanceJointsIdGenerator;

    struct BallJointRecord
    {
        BallJoint Joint;
        PhysicsJointHandle Handle;
    };
    std::vector<BallJointRecord> FBallJoints;

    IdGenerator FBallJointsIdGenerator;

    struct HingeJointRecord
    {
        HingeJoint Joint;
        PhysicsJointHandle Handle;
    };
    std::vector<HingeJointRecord> FHingeJoints;

    IdGenerator FHingeJointsIdGenerator;
};

} // namespace Physics
} // namespace ECSEngine