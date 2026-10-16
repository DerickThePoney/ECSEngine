#include "stdafx.h"

#include "Joints.h"

namespace ECSEngine
{
namespace Physics
{
void DistanceJoint::Initialize(const PhysicsBodyHandle& BodyA, const PhysicsBodyHandle& BodyB, const DistanceJointConfig& Config)
{
    FBodyA = BodyA;
    FBodyB = BodyB;
    FConfig = Config;

    FImpulse = 0;
    bIsland = false;
}

void DistanceJoint::Destroy()
{
    FBodyA = PhysicsBodyHandle{ };
    FBodyB = PhysicsBodyHandle{ };
}
} // namespace Physics
} // namespace ECSEngine
