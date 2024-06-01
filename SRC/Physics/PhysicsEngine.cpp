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

        // damping
        body->FVelocity *= 1.f / (1.f + parDeltaTime * body->FLinearDamping);

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
    }
}

const PhysicsBodyHandle PhysicsEngine::CreateNewPhysicsBody(const mat4& Transform, const PhysicsBodyConfig& BodyConfig)
{
    PhysicsBodyHandle newHandle;

    RigidBody* newBody = new RigidBody;
    newBody->FPosition = Transform.Column(3).xyz();
    newBody->FMass = BodyConfig.FMass;
    newBody->FLinearDamping = BodyConfig.FLinearDamping;
    newBody->FAngularDamping = BodyConfig.FAngularDamping;
    if (!BodyConfig.FApplyGravity)
        newBody->FGravityScale = 0.f;

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

    if (bTreatAsAcceleration)
    {
        body->FAccelerationDueToForces += Force;
    }
    else
    {
        body->FAccelerationDueToForces += Force * body->FInvMass;
    }

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

    if (bTreatAsVelocityChange)
    {
        body->FVelocity += Impulse;
    }
    else
    {
        body->FVelocity += Impulse * body->FInvMass;
    }
    return false;
}

} // namespace Physics
} // namespace ECSEngine
