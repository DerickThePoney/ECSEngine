#include "stdafx.h"

#include "PhysicsJointsManager.h"

#include "Common/Logger.h"

namespace ECSEngine
{
namespace Physics
{

PhysicsJointHandle PhysicsJointManager::CreateDistanceJoint(const PhysicsBodyHandle& BodyA, const PhysicsBodyHandle& BodyB, const DistanceJointConfig& Config)
{
    u16 Id = GetNextDistanceJointId();
    if (Id == (u16)~0)
    {
        return PhysicsJointHandle{ };
    }

    if (Id > FDistanceJoints.size())
    {
        FDistanceJoints.resize(Id + 1);
        FDistanceJoints[Id].Handle.FIndex = Id;
        FDistanceJoints[Id].Handle.FType = (u8)EJointType::DISTANCE;
        FDistanceJoints[Id].Handle.FGeneration = 0;
    }

    // Fill up joint data
    FDistanceJoints[Id].Joint.Initialize(BodyA, BodyB, Config);

    return FDistanceJoints[Id].Handle;
}

void PhysicsJointManager::DestroyDistanceJoint(const PhysicsJointHandle& Handle)
{
    if (!Handle.IsValid())
    {
        return;
    }

    if (Handle.GetTypeAsEnum() != EJointType::DISTANCE)
    {
        LOG_ERROR(std::format("Trying to delete a distance joint (type {}) with a handle for type {}", (u8)EJointType::DISTANCE, Handle.FType).c_str());
        return;
    }

    if (Handle.FIndex > FDistanceJoints.size())
    {
        LOG_ERROR(std::format("Distance joint handle index is out of bound: {} - Count {}", Handle.FIndex, FDistanceJoints.size()).c_str());
        return;
    }

    // Just Increase the generation. Need reset on the actual joint ?
    FDistanceJoints[Handle.FIndex].Handle.FGeneration++;
    FDistanceJoints[Handle.FIndex].Joint.Destroy();

    FDistanceJointsIdGenerator.ReleaseId(Handle.FIndex);
}

u16 PhysicsJointManager::GetNextDistanceJointId()
{
    u32 Id = FDistanceJointsIdGenerator.GetNextId();
    if ((Id & 0xFFFF0000) > 0)
    {
        LOG_ERROR("Already filled the distance joints buffer");
        return (u16)~0;
    }
    return (u16)Id;
}
} // namespace Physics
} // namespace ECSEngine
