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
        DestroyJoint(jointHandle);
    }

    Body->FJoints.clear();
}

u16 GetNextJointId(IdGenerator& Generator)
{
    u32 Id = Generator.GetNextId();
    if ((Id & 0xFFFF0000) > 0)
    {
        LOG_ERROR("Already filled the joints buffer for this buffer");
        AssertNotReached();
        return (u16)~0;
    }
    return (u16)Id;
}

template<typename Container, typename JointConfig>
PhysicsJointHandle
CreateJoint(const PhysicsBodyHandle& BodyA, const PhysicsBodyHandle& BodyB, Container& JointContainer, const JointConfig& Config, EJointType JointType, IdGenerator& Generator)
{
    RigidBody* bodyA = PhysicsEngine::Instance().GetRigidBody(BodyA);
    RigidBody* bodyB = PhysicsEngine::Instance().GetRigidBody(BodyB);

    if (bodyA == nullptr || bodyB == nullptr)
    {
        LOG_ERROR("Trying to attach stale bodies...");
        return PhysicsJointHandle{ };
    }

    u16 Id = GetNextJointId(Generator);

    if (Id == (u16)~0)
    {
        return PhysicsJointHandle{ };
    }

    if (Id >= JointContainer.size())
    {
        JointContainer.resize(Id + 1);
        JointContainer[Id].Handle.FIndex = Id;
        JointContainer[Id].Handle.FType = (u8)JointType;
        JointContainer[Id].Handle.FGeneration = 0;
    }

    // Fill up joint data
    JointContainer[Id].Joint.Initialize(BodyA, BodyB, Config);
    bodyA->FJoints.push_back(JointContainer[Id].Handle);
    bodyB->FJoints.push_back(JointContainer[Id].Handle);

    bodyA->SetAwake(true);
    bodyB->SetAwake(true);

    return JointContainer[Id].Handle;
}

PhysicsJointHandle PhysicsJointsManager::CreateDistanceJoint(const PhysicsBodyHandle& BodyA, const PhysicsBodyHandle& BodyB, const DistanceJointConfig& Config)
{
    if (BodyA == BodyB)
    {
        LOG_ERROR("Nice try young padawan. But attaching the same body to the same joint is not supported !");
        return PhysicsJointHandle{ };
    }

    return CreateJoint(BodyA, BodyB, FDistanceJoints, Config, EJointType::DISTANCE, FDistanceJointsIdGenerator);
}

PhysicsJointHandle PhysicsJointsManager::CreateBallJoint(const PhysicsBodyHandle& BodyA, const PhysicsBodyHandle& BodyB, const BallJointConfig& Config)
{
    if (BodyA == BodyB)
    {
        LOG_ERROR("Nice try young padawan. But attaching the same body to the same joint is not supported !");
        return PhysicsJointHandle{ };
    }

    return CreateJoint(BodyA, BodyB, FBallJoints, Config, EJointType::BALL, FBallJointsIdGenerator);
}

PhysicsJointHandle PhysicsJointsManager::CreateHingeJoint(const PhysicsBodyHandle& BodyA, const PhysicsBodyHandle& BodyB, const HingeJointConfig& Config)
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

    return CreateJoint(BodyA, BodyB, FHingeJoints, Config, EJointType::HINGE, FHingeJointsIdGenerator);
}

template<typename Container>
void DestroyJointTemplate(const PhysicsJointHandle& Handle, Container& JointContainer, IdGenerator& Generator, EJointType JointType)
{
    if (Handle.GetTypeAsEnum() != JointType)
    {
        LOG_ERROR(std::format("Trying to delete a joint (type {}) with a handle for type {}", (u8)JointType, Handle.FType).c_str());
        return;
    }

    if (Handle.FIndex >= JointContainer.size())
    {
        LOG_ERROR(std::format("Joint handle index is out of bound: {} - Count {}", Handle.FIndex, JointContainer.size()).c_str());
        return;
    }

    if (Handle.FGeneration != JointContainer[Handle.FIndex].Handle.FGeneration)
    {
        LOG_ERROR(
              std::format("Trying to destroy an outdated joint: Handle Generation {} - Current Generation {}", Handle.FGeneration, JointContainer[Handle.FIndex].Handle.FGeneration)
                    .c_str());
        return;
    }

    // Detach the rigidbodies
    RigidBody* bodyA = PhysicsEngine::Instance().GetRigidBody(JointContainer[Handle.FIndex].Joint.FBodyA);
    RigidBody* bodyB = PhysicsEngine::Instance().GetRigidBody(JointContainer[Handle.FIndex].Joint.FBodyB);
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
    JointContainer[Handle.FIndex].Handle.FGeneration++;
    JointContainer[Handle.FIndex].Joint.Destroy();

    Generator.ReleaseId(Handle.FIndex);
}

void PhysicsJointsManager::DestroyJoint(const PhysicsJointHandle& Handle)
{
    if (!Handle.IsValid())
    {
        return;
    }

    switch (Handle.GetTypeAsEnum())
    {
    case EJointType::DISTANCE:
    {
        DestroyJointTemplate(Handle, FDistanceJoints, FDistanceJointsIdGenerator, EJointType::DISTANCE);
        break;
    }
    case EJointType::BALL:
    {
        DestroyJointTemplate(Handle, FBallJoints, FBallJointsIdGenerator, EJointType::BALL);
        break;
    }
    case EJointType::HINGE:
    {
        DestroyJointTemplate(Handle, FHingeJoints, FHingeJointsIdGenerator, EJointType::HINGE);
        break;
    }
    default:
        AssertNotReached();
    }
}

JointBase* PhysicsJointsManager::GetJoint(const PhysicsJointHandle& Handle)
{
    if (!Handle.IsValid())
        return nullptr;

    switch (Handle.GetTypeAsEnum())
    {
    case EJointType::DISTANCE:
    {
        if (Handle.FIndex >= FDistanceJoints.size())
            return nullptr;
        if (FDistanceJoints[Handle.FIndex].Handle != Handle)
            return nullptr;
        return &FDistanceJoints[Handle.FIndex].Joint;
        break;
    }
    case EJointType::BALL:
    {
        if (Handle.FIndex >= FBallJoints.size())
            return nullptr;
        if (FBallJoints[Handle.FIndex].Handle != Handle)
            return nullptr;
        return &FBallJoints[Handle.FIndex].Joint;
        break;
    }
    case EJointType::HINGE:
    {
        if (Handle.FIndex >= FHingeJoints.size())
            return nullptr;
        if (FHingeJoints[Handle.FIndex].Handle != Handle)
            return nullptr;
        return &FHingeJoints[Handle.FIndex].Joint;
        break;
    }
    default:
        AssertNotReached();
    }

    return nullptr;
}

void PhysicsJointsManager::PreUpdate()
{
    foreachitem(joint, FDistanceJoints)
    {
        joint.Joint.bIsland = false;
    }

    foreachitem(joint, FBallJoints)
    {
        joint.Joint.bIsland = false;
    }

    foreachitem(joint, FHingeJoints)
    {
        joint.Joint.bIsland = false;
    }
}
} // namespace Physics
} // namespace ECSEngine
