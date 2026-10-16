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
    void DestroyDistanceJoint(const PhysicsJointHandle& Handle);

    JointBase* GetJoint(const PhysicsJointHandle& Handle);

    void PreUpdate();

private:
    u16 GetNextDistanceJointId();

private:
    struct DistanceJointRecord
    {
        DistanceJoint Joint;
        PhysicsJointHandle Handle;
    };
    std::vector<DistanceJointRecord> FDistanceJoints;

    IdGenerator FDistanceJointsIdGenerator;
};

} // namespace Physics
} // namespace ECSEngine