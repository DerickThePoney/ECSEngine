#include "stdafx.h"

#include "PhysicsJointsManager.h"

#include "Common/Logger.h"
#include "PhysicsEngine.h"

namespace ECSEngine
{
namespace Physics
{
void PhysicsJointsManager::RemoveBody(RigidBody* Body)
{
    foreachitemconst(jointHandle, Body->FJoints)
    {
        switch (jointHandle.GetTypeAsEnum())
        {
        case EJointType::DISTANCE:
        {
            DestroyDistanceJoint(jointHandle);
            break;
        }
        default:
            AssertNotReachedMsg("Not implemented yet");
        }
    }

    Body->FJoints.clear();
}

void PhysicsJointsManager::Intialize(Island* parIsland)
{
}

void PhysicsJointsManager::PreSolve()
{
}

void PhysicsJointsManager::Solve()
{
}

PhysicsJointHandle PhysicsJointsManager::CreateDistanceJoint(const PhysicsBodyHandle& BodyA, const PhysicsBodyHandle& BodyB, const DistanceJointConfig& Config)
{
    if (BodyA == BodyB)
    {
        LOG_ERROR("Nice try young padawan. But attaching the same body to the same joint is not supported !");
        return PhysicsJointHandle{ };
    }

    RigidBody* bodyA = PhysicsEngine::Instance().GetRigidBody(BodyA);
    RigidBody* bodyB = PhysicsEngine::Instance().GetRigidBody(BodyB);

    if (bodyA == nullptr || bodyB == nullptr)
    {
        LOG_ERROR("Trying to attach stale bodies...");
        return PhysicsJointHandle{ };
    }

    u16 Id = GetNextDistanceJointId();
    if (Id == (u16)~0)
    {
        return PhysicsJointHandle{ };
    }

    if (Id >= FDistanceJoints.size())
    {
        FDistanceJoints.resize(Id + 1);
        FDistanceJoints[Id].Handle.FIndex = Id;
        FDistanceJoints[Id].Handle.FType = (u8)EJointType::DISTANCE;
        FDistanceJoints[Id].Handle.FGeneration = 0;
    }

    // Fill up joint data
    FDistanceJoints[Id].Joint.Initialize(BodyA, BodyB, Config);
    bodyA->FJoints.push_back(FDistanceJoints[Id].Handle);
    bodyB->FJoints.push_back(FDistanceJoints[Id].Handle);

    bodyA->SetAwake(true);
    bodyB->SetAwake(true);

    return FDistanceJoints[Id].Handle;
}

void PhysicsJointsManager::DestroyDistanceJoint(const PhysicsJointHandle& Handle)
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

    if (Handle.FIndex >= FDistanceJoints.size())
    {
        LOG_ERROR(std::format("Distance joint handle index is out of bound: {} - Count {}", Handle.FIndex, FDistanceJoints.size()).c_str());
        return;
    }

    if (Handle.FGeneration != FDistanceJoints[Handle.FIndex].Handle.FGeneration)
    {
        LOG_ERROR(std::format(
              "Trying to destroy an outdated joint: Handle Generation {} - Current Generation {}", Handle.FGeneration, FDistanceJoints[Handle.FIndex].Handle.FGeneration)
                    .c_str());
        return;
    }

    // Detach the rigidbodies
    RigidBody* bodyA = PhysicsEngine::Instance().GetRigidBody(FDistanceJoints[Handle.FIndex].Joint.FHeader.FBodyA);
    RigidBody* bodyB = PhysicsEngine::Instance().GetRigidBody(FDistanceJoints[Handle.FIndex].Joint.FHeader.FBodyB);
    if (bodyA != nullptr)
    {
        for (auto it = bodyA->FJoints.begin(); it != bodyA->FJoints.end(); ++it)
        {
            if (*it == Handle)
            {
                bodyA->FJoints.erase(it);
                break;
            }
        }

        bodyA->SetAwake(true);
    }

    if (bodyB != nullptr)
    {
        for (auto it = bodyB->FJoints.begin(); it != bodyB->FJoints.end(); ++it)
        {
            if (*it == Handle)
            {
                bodyB->FJoints.erase(it);
                break;
            }
        }
        bodyB->SetAwake(true);
    }

    // Just Increase the generation.
    FDistanceJoints[Handle.FIndex].Handle.FGeneration++;
    FDistanceJoints[Handle.FIndex].Joint.Destroy();

    FDistanceJointsIdGenerator.ReleaseId(Handle.FIndex);
}

u16 PhysicsJointsManager::GetNextDistanceJointId()
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
