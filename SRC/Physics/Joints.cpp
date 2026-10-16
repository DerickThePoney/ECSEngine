#include "stdafx.h"

#include "Joints.h"

namespace ECSEngine
{
namespace Physics
{
void DistanceJoint::Initialize(const PhysicsBodyHandle& BodyA, const PhysicsBodyHandle& BodyB, const DistanceJointConfig& Config)
{
    FHeader.FBodyA = BodyA;
    FHeader.FBodyB = BodyB;
    FConfig = Config;

    FImpulse = 0;
    FHeader.bIsland = false;
}

void DistanceJoint::Destroy()
{
    FHeader.FBodyA = PhysicsBodyHandle{ };
    FHeader.FBodyB = PhysicsBodyHandle{ };
}
} // namespace Physics
} // namespace ECSEngine
