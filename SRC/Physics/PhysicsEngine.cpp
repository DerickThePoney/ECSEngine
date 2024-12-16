#include "stdafx.h"

#include "PhysicsEngine.h"

#include "PhysicsBodyConfig.h"
#include "PhysicsBodyHandle.h"

namespace ECSEngine
{
namespace Physics
{
PhysicsEngine::PhysicsEngine()
    : Singleton()
{
}

PhysicsEngine::~PhysicsEngine()
{
}

void PhysicsEngine::Initialize(const PhysicsEngineConfiguration& PhysicsConfig)
{
    SetConfig(PhysicsConfig);
}

void PhysicsEngine::Cleanup()
{
    FRigidbodies.clear();
}

void PhysicsEngine::UpdatePhysics(float parDeltaTime)
{
    // MAKE THE MOVED BODIES ARRAY A CLASS ONE AND ALLOW KINEMATIC UPDATES
    // integrate velocity
    foreachitem(bodyHandle, FPhysicsRigidbodies)
    {
        if (!bodyHandle.IsValid())
        {
            continue;
        }

        RigidBody* body = FRigidbodies[bodyHandle.FId].get();
        if (body == nullptr)
        {
            continue;
        }

        body->FVelocity += (body->FAccelerationDueToForces + body->FGravityScale * FConfig.FGravityValue * vec3(0.f, -1.f, 0.f)) * parDeltaTime;
        body->FRotationVelocity += body->FInverseInertiaTensorWorld * body->FTorque * parDeltaTime;

        // damping
        body->FVelocity *= 1.f / (1.f + parDeltaTime * body->FLinearDamping);
        body->FRotationVelocity *= 1.f / (1.f + parDeltaTime * body->FAngularDamping);

        body->FAccelerationDueToForces = vec3(0.f);
        body->FTorque = vec3(0.f);
    }

    struct MovedBodies
    {
        RigidBody* Body = nullptr;
        vec3 displacement;
    };

    std::vector<MovedBodies> MovedBodiesArray;

    // integrate position
    foreachitem(bodyHandle, FPhysicsRigidbodies)
    {
        if (!bodyHandle.IsValid())
        {
            continue;
        }

        RigidBody* body = FRigidbodies[bodyHandle.FId].get();
        if (body == nullptr)
        {
            continue;
        }

        const vec3 displacement = body->FVelocity * parDeltaTime;
        body->FPosition += displacement;
        body->FOrientation = AddVectorToQuaternion(body->FOrientation, body->FRotationVelocity * parDeltaTime);

        MovedBodiesArray.push_back({ body, displacement });
    }

    // Update inertia tensor
    foreachitem(bodyHandle, FPhysicsRigidbodies)
    {
        if (!bodyHandle.IsValid())
        {
            continue;
        }

        RigidBody* body = FRigidbodies[bodyHandle.FId].get();
        if (body == nullptr)
        {
            continue;
        }

        UpdateInertiaTransform(body);
    }

    foreachitem(movedBody, MovedBodiesArray)
    {
        FBroadPhase.MoveBody(movedBody.Body, movedBody.displacement);
    }

    FContactManager.FindNewContacts(FBroadPhase);

    FContactManager.CollideContacts(this, FBroadPhase);

#ifdef PERFORM_SECURITY_CHECKS
    FBroadPhase.DebugBroadPhase();
    FContactManager.DebugDrawContacts(FBroadPhase);
#endif
}

const PhysicsBodyHandle PhysicsEngine::CreateNewPhysicsBody(const mat4& Transform, const PhysicsBodyConfig& BodyConfig)
{
    PhysicsBodyHandle newHandle;

    RigidBody* newBody = new RigidBody;
    InitializeBody(newBody, Transform, BodyConfig);

    newHandle.FId = FHandleGenerator.GetNextId();
    newBody->FHandle = newHandle;

    if (FRigidbodies.size() > newHandle.FId)
    {
        AlwaysCheckedAssert(FRigidbodies[newHandle.FId] == nullptr);
        FRigidbodies[newHandle.FId].reset(newBody);
    }
    else
    {
        AssertRelease(FRigidbodies.size() == newHandle.FId);
        FRigidbodies.emplace_back(newBody);
    }

    FBroadPhase.AddNewBody(newBody);

    switch (newBody->FMoveabilityType)
    {
    case EPhysicsMoveability::STATIC:
        FStaticRigidbodies.insert(newHandle);
        break;
    case EPhysicsMoveability::KINEMATIC:
        FKinematicRigidbodies.insert(newHandle);
        break;
    case EPhysicsMoveability::PHYICS_ENABLED:
        FPhysicsRigidbodies.insert(newHandle);
        break;
    default:
        AssertNotReached();
        break;
    }
    return newHandle;
}

bool PhysicsEngine::DestroyPhysicsBody(const PhysicsBodyHandle& Handle)
{
    if (!Handle.IsValid())
    {
        return false;
    }

    if (Handle.FId > FRigidbodies.size())
    {
        return false;
    }

    FBroadPhase.RemoveBody(FRigidbodies[Handle.FId].get());
    FRigidbodies[Handle.FId].reset(nullptr);

    return true;
}

#pragma region BodyGettersAndSetters
bool PhysicsEngine::SetBodyPosition(const PhysicsBodyHandle& Handle, const vec3& Position)
{
    RigidBody* body = GetRigidBody(Handle);
    if (body == nullptr)
    {
        return false;
    }

    body->FPosition = Position;
    return true;
}

bool PhysicsEngine::SetBodyVelocity(const PhysicsBodyHandle& Handle, const vec3& Velocity)
{
    RigidBody* body = GetRigidBody(Handle);
    if (body == nullptr)
    {
        return false;
    }

    body->FVelocity = Velocity;
    return true;
}

bool PhysicsEngine::SetBodyOrientation(const PhysicsBodyHandle& Handle, const quat& Orientation)
{
    RigidBody* body = GetRigidBody(Handle);
    if (body == nullptr)
    {
        return false;
    }

    body->FOrientation = Orientation;
    return true;
}

bool PhysicsEngine::SetBodyRotationVelocity(const PhysicsBodyHandle& Handle, const vec3& RotationVelocity)
{
    RigidBody* body = GetRigidBody(Handle);
    if (body == nullptr)
    {
        return false;
    }

    body->FRotationVelocity = RotationVelocity;
    return true;
}

bool PhysicsEngine::GetBodyPosition(const PhysicsBodyHandle& Handle, vec3& Position) const
{
    const RigidBody* body = GetRigidBody(Handle);
    if (body == nullptr)
    {
        return false;
    }

    Position = body->FPosition;
    return true;
}

bool PhysicsEngine::GetBodyVelocity(const PhysicsBodyHandle& Handle, vec3& Velocity) const
{
    const RigidBody* body = GetRigidBody(Handle);
    if (body == nullptr)
    {
        return false;
    }

    Velocity = body->FVelocity;
    return true;
}

bool PhysicsEngine::GetBodyOrientation(const PhysicsBodyHandle& Handle, quat& Orientation) const
{
    const RigidBody* body = GetRigidBody(Handle);
    if (body == nullptr)
    {
        return false;
    }

    Orientation = body->FOrientation;
    return true;
}

bool PhysicsEngine::GetBodyRotationVelocity(const PhysicsBodyHandle& Handle, vec3& RotationVelocity) const
{
    const RigidBody* body = GetRigidBody(Handle);
    if (body == nullptr)
    {
        return false;
    }

    RotationVelocity = body->FRotationVelocity;
    return true;
}

#pragma endregion BodyGettersAndSetters

#pragma region ForcesAndTorques
bool PhysicsEngine::AddForceToBody(const PhysicsBodyHandle& Handle, const vec3& Force, bool bTreatAsAcceleration)
{
    RigidBody* body = GetRigidBody(Handle);
    if (body == nullptr)
    {
        return false;
    }

    body->AddForce(Force, bTreatAsAcceleration);

    return true;
}

bool PhysicsEngine::AddForceAtPointToBody(const PhysicsBodyHandle& Handle, const vec3& Force, const vec3& Point, bool bTreatPointAsLocalCoord, bool bTreatAsAcceleration)
{
    RigidBody* body = GetRigidBody(Handle);
    if (body == nullptr)
    {
        return false;
    }

    vec3 PointToUse = Point;
    if (bTreatPointAsLocalCoord)
    {
        PointToUse = (Translation(body->FPosition) * (mat4)body->FOrientation * vec4::MakeHomogeneousPositionVec4(Point)).xyz();
    }

    PointToUse -= body->FPosition;

    body->AddForce(Force, bTreatAsAcceleration);

    vec3 TorqueToAdd = Cross(PointToUse, Force);

    body->AddTorque(TorqueToAdd);

    return true;
}

bool PhysicsEngine::AddTorqueToBody(const PhysicsBodyHandle& Handle, const vec3& Torque)
{
    RigidBody* body = GetRigidBody(Handle);
    if (body == nullptr)
    {
        return false;
    }

    body->AddTorque(Torque);
    return true;
}
#pragma endregion ForcesAndTorques

#pragma region Impulses
bool PhysicsEngine::AddImpulseToBody(const PhysicsBodyHandle& Handle, const vec3& Impulse, bool bTreatAsVelocityChange)
{
    RigidBody* body = GetRigidBody(Handle);
    if (body == nullptr)
    {
        return false;
    }

    body->AddImpulse(Impulse, bTreatAsVelocityChange);
    return true;
}

bool PhysicsEngine::AddImpulseAtPointToBody(const PhysicsBodyHandle& Handle, const vec3& Impulse, const vec3& Point, bool bTreatPointAsLocalCoord, bool bTreatAsVelocityChange)
{
    RigidBody* body = GetRigidBody(Handle);
    if (body == nullptr)
    {
        return false;
    }

    vec3 PointToUse = Point;
    if (bTreatPointAsLocalCoord)
    {
        PointToUse = (Translation(body->FPosition) * (mat4)body->FOrientation * vec4::MakeHomogeneousPositionVec4(Point)).xyz();
    }

    PointToUse -= body->FPosition;
    body->AddImpulse(Impulse, bTreatAsVelocityChange);

    vec3 RotationImpulseToAdd = Cross(PointToUse, Impulse);
    body->AddRotationImpulse(RotationImpulseToAdd, bTreatAsVelocityChange);
    return true;
}

bool PhysicsEngine::AddRotationImpulseToBody(const PhysicsBodyHandle& Handle, const vec3& RotationImpulse, bool bTreatAsRotationVelocityChange)
{
    RigidBody* body = GetRigidBody(Handle);
    if (body == nullptr)
    {
        return false;
    }

    body->AddRotationImpulse(RotationImpulse, bTreatAsRotationVelocityChange);
    return true;
}

#pragma endregion Impulses

void PhysicsEngine::InitializeBody(RigidBody* Body, const mat4& Transform, const PhysicsBodyConfig& BodyConfig)
{
    AssertRelease(Body != nullptr);

    // Moveability
    Body->FMoveabilityType = BodyConfig.FMoveability;

    // Init position and orientation
    Body->FPosition = Transform.Column(3).xyz();
    Body->FOrientation = quat::FromMat4(Transform);

    // Init Mass and Inertia
    if (BodyConfig.FAutoComputeMass)
    {
        Body->FMass = BodyConfig.FShape.ComputeMass(BodyConfig.FDensity);
    }
    else
    {
        Body->FMass = BodyConfig.FMass;
    }

    Body->FInvMass = (BodyConfig.FMass != 0.f) ? 1.f / BodyConfig.FMass : 1.f;
    Body->FInertiaTensor = BodyConfig.FShape.ComputeInertiaTensor(Body->FMass);
    Body->FInverseInertiaTensor = Invert(Body->FInertiaTensor);

    Body->FCollisionShape = BodyConfig.FShape;

    // Init damping coefficients
    Body->FLinearDamping = BodyConfig.FLinearDamping;
    Body->FAngularDamping = BodyConfig.FAngularDamping;
    if (!BodyConfig.FApplyGravity)
        Body->FGravityScale = 0.f;
}

void PhysicsEngine::UpdateInertiaTransform(RigidBody* Body)
{
    AssertRelease(Body != nullptr);

    // Normalize orientation
    Body->FOrientation = Normalize(Body->FOrientation);

    // recompute the inverse inertia tensor in world coordinate using Mt' = Mb * Mt * Mb^-1
    mat3 worldRotation = GetRotation((mat4)Body->FOrientation);
    Body->FInverseInertiaTensorWorld = worldRotation * Body->FInverseInertiaTensor * Transpose(worldRotation);
}

RigidBody* PhysicsEngine::GetRigidBody(const PhysicsBodyHandle& Handle)
{
    if (!Handle.IsValid())
    {
        return nullptr;
    }

    if (Handle.FId > FRigidbodies.size())
    {
        return nullptr;
    }

    return FRigidbodies[Handle.FId].get();
}

const RigidBody* PhysicsEngine::GetRigidBody(const PhysicsBodyHandle& Handle) const
{
    if (!Handle.IsValid())
    {
        return nullptr;
    }

    if (Handle.FId > FRigidbodies.size())
    {
        return nullptr;
    }

    return FRigidbodies[Handle.FId].get();
}

} // namespace Physics
} // namespace ECSEngine
