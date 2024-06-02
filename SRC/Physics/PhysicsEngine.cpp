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
    // integrate velocity
    foreachitem(body, FRigidbodies)
    {
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
    }

    // integrate position
    foreachitem(body, FRigidbodies)
    {
        if (body == nullptr)
        {
            continue;
        }

        body->FPosition += body->FVelocity * parDeltaTime;
        body->FOrientation = AddVectorToQuaternion(body->FOrientation, body->FRotationVelocity * parDeltaTime);
    }

    // Update inertia tensor
    foreachitem(body, FRigidbodies)
    {
        if (body == nullptr)
        {
            continue;
        }

        UpdateInertiaTransform(body.get());
    }
}

const PhysicsBodyHandle PhysicsEngine::CreateNewPhysicsBody(const mat4& Transform, const PhysicsBodyConfig& BodyConfig)
{
    PhysicsBodyHandle newHandle;

    RigidBody* newBody = new RigidBody;
    InitializeBody(newBody, Transform, BodyConfig);

    newHandle.FId = FHandleGenerator.GetNextId();

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

    FRigidbodies[Handle.FId].reset(nullptr);

    return true;
}

bool PhysicsEngine::SetBodyPosition(const PhysicsBodyHandle& Handle, const vec3& Position)
{
    if (!Handle.IsValid())
    {
        return false;
    }

    if (Handle.FId > FRigidbodies.size())
    {
        return false;
    }

    std::unique_ptr<RigidBody>& body = FRigidbodies[Handle.FId];
    if (body == nullptr)
    {
        return false;
    }

    body->FPosition = Position;
    return true;
}

bool PhysicsEngine::SetBodyVelocity(const PhysicsBodyHandle& Handle, const vec3& Velocity)
{
    if (!Handle.IsValid())
    {
        return false;
    }

    if (Handle.FId > FRigidbodies.size())
    {
        return false;
    }

    std::unique_ptr<RigidBody>& body = FRigidbodies[Handle.FId];
    if (body == nullptr)
    {
        return false;
    }

    body->FVelocity = Velocity;
    return true;
}

bool PhysicsEngine::SetBodyOrientation(const PhysicsBodyHandle& Handle, const quat& Orientation)
{
    if (!Handle.IsValid())
    {
        return false;
    }

    if (Handle.FId > FRigidbodies.size())
    {
        return false;
    }

    std::unique_ptr<RigidBody>& body = FRigidbodies[Handle.FId];
    if (body == nullptr)
    {
        return false;
    }

    body->FOrientation = Orientation;
    return true;
}

bool PhysicsEngine::SetBodyRotationVelocity(const PhysicsBodyHandle& Handle, const vec3& RotationVelocity)
{
    if (!Handle.IsValid())
    {
        return false;
    }

    if (Handle.FId > FRigidbodies.size())
    {
        return false;
    }

    std::unique_ptr<RigidBody>& body = FRigidbodies[Handle.FId];
    if (body == nullptr)
    {
        return false;
    }

    body->FRotationVelocity = RotationVelocity;
    return true;
}

bool PhysicsEngine::GetBodyPosition(const PhysicsBodyHandle& Handle, vec3& Position) const
{
    if (!Handle.IsValid())
    {
        return false;
    }

    if (Handle.FId > FRigidbodies.size())
    {
        return false;
    }

    const std::unique_ptr<RigidBody>& body = FRigidbodies[Handle.FId];
    if (body == nullptr)
    {
        return false;
    }
    Position = body->FPosition;
    return true;
}

bool PhysicsEngine::GetBodyVelocity(const PhysicsBodyHandle& Handle, vec3& Velocity) const
{
    if (!Handle.IsValid())
    {
        return false;
    }

    if (Handle.FId > FRigidbodies.size())
    {
        return false;
    }

    const std::unique_ptr<RigidBody>& body = FRigidbodies[Handle.FId];
    if (body == nullptr)
    {
        return false;
    }
    Velocity = body->FVelocity;
    return true;
}

bool PhysicsEngine::GetBodyOrientation(const PhysicsBodyHandle& Handle, quat& Orientation) const
{
    if (!Handle.IsValid())
    {
        return false;
    }

    if (Handle.FId > FRigidbodies.size())
    {
        return false;
    }

    const std::unique_ptr<RigidBody>& body = FRigidbodies[Handle.FId];
    if (body == nullptr)
    {
        return false;
    }
    Orientation = body->FOrientation;
    return true;
}

bool PhysicsEngine::GetBodyRotationVelocity(const PhysicsBodyHandle& Handle, vec3& RotationVelocity) const
{
    if (!Handle.IsValid())
    {
        return false;
    }

    if (Handle.FId > FRigidbodies.size())
    {
        return false;
    }

    const std::unique_ptr<RigidBody>& body = FRigidbodies[Handle.FId];
    if (body == nullptr)
    {
        return false;
    }
    RotationVelocity = body->FRotationVelocity;
    return true;
}

bool PhysicsEngine::AddForceToBody(const PhysicsBodyHandle& Handle, const vec3& Force, bool bTreatAsAcceleration)
{
    if (!Handle.IsValid())
    {
        return false;
    }

    if (Handle.FId > FRigidbodies.size())
    {
        return false;
    }

    std::unique_ptr<RigidBody>& body = FRigidbodies[Handle.FId];
    if (body == nullptr)
    {
        return false;
    }

    body->AddForce(Force, bTreatAsAcceleration);

    return true;
}

// TODO MOVE ADD FORCES / IMPULSES / TORQUES TO RIGIDBODY STRUCT

bool PhysicsEngine::AddForceAtPointToBody(const PhysicsBodyHandle& Handle, const vec3& Force, const vec3& Point, bool bTreatPointAsLocal, bool bTreatAsAcceleration)
{
    if (!Handle.IsValid())
    {
        return false;
    }

    if (Handle.FId > FRigidbodies.size())
    {
        return false;
    }

    std::unique_ptr<RigidBody>& body = FRigidbodies[Handle.FId];
    if (body == nullptr)
    {
        return false;
    }

    vec3 PointToUse = Point;
    if (bTreatPointAsLocal)
    {
        PointToUse = (Translation(body->FPosition) * (mat4)body->FOrientation * vec4::MakeHomogeneousPositionVec4(Point)).xyz();
    }

    PointToUse -= body->FPosition;

    body->AddForce(Force, bTreatAsAcceleration);

    vec3 TorqueToAdd = Cross(PointToUse, Force);

    body->AddTorque(TorqueToAdd);

    return true;
}

bool PhysicsEngine::AddImpulseToBody(const PhysicsBodyHandle& Handle, const vec3& Impulse, bool bTreatAsVelocityChange)
{
    if (!Handle.IsValid())
    {
        return false;
    }

    if (Handle.FId > FRigidbodies.size())
    {
        return false;
    }

    std::unique_ptr<RigidBody>& body = FRigidbodies[Handle.FId];
    if (body == nullptr)
    {
        return false;
    }

    body->AddImpulse(Impulse, bTreatAsVelocityChange);
    return false;
}

void PhysicsEngine::InitializeBody(RigidBody* Body, const mat4& Transform, const PhysicsBodyConfig& BodyConfig)
{
    AssertRelease(Body != nullptr);

    // Init position and orientation
    Body->FPosition = Transform.Column(3).xyz();
    Body->FOrientation = quat::FromMat4(Transform);

    // Init Mass and Inertia
    Body->FMass = BodyConfig.FMass;
    Body->FInvMass = (BodyConfig.FMass != 0.f) ? 1.f / BodyConfig.FMass : 1.f;
    Body->FInertiaTensor = mat3::Identity();
    Body->FInverseInertiaTensor = mat3::Identity();

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

} // namespace Physics
} // namespace ECSEngine
