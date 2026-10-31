#include "stdafx.h"

#include "JointBatchRegistry.h"

#include "BallJointBatch.h"
#include "DistanceJointBatch.h"
#include "HingeJointBatch.h"

namespace ECSEngine
{
namespace Physics
{

void JointBatchRegistry::Initialize()
{
    Shutdown();
    FBatches[(u8)EJointType::DISTANCE] = new DistanceJointBatch();
    FBatches[(u8)EJointType::BALL] = new BallJointBatch();
    FBatches[(u8)EJointType::HINGE] = new HingeJointBatch();
}

void JointBatchRegistry::Shutdown()
{
    for (u8 i = 0; i < (u8)EJointType::LENGTH; ++i)
    {
        delete FBatches[i];
        FBatches[i] = nullptr;
    }
}

IJointBatch* JointBatchRegistry::Get(EJointType type) const
{
    AssertRelease((u8)type < (u8)EJointType::LENGTH);
    return FBatches[(u8)type];
}

} // namespace Physics
} // namespace ECSEngine
