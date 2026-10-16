#pragma once
#include "Joints.h"

namespace ECSEngine
{
namespace Physics
{

class PhysicsJointManager
{
public:
    PhysicsJointHandle CreateDistanceJoint(const PhysicsBodyHandle& BodyA, const PhysicsBodyHandle& BodyB, const DistanceJointConfig& Config);
    void DestroyDistanceJoint(const PhysicsJointHandle& Handle);

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