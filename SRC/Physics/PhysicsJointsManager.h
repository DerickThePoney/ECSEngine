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

    void DestroyJoint(const PhysicsJointHandle& Handle);

    JointBase* GetJoint(const PhysicsJointHandle& Handle);

    void PreUpdate();

private:
    void DestroyDistanceJoint(const PhysicsJointHandle& Handle);
    u16 GetNextDistanceJointId();

    void DestroyBallJoint(const PhysicsJointHandle& Handle);
    u16 GetNextBallJointId();

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
};

} // namespace Physics
} // namespace ECSEngine