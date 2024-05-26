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
}

const PhysicsBodyHandle PhysicsEngine::CreateNewPhysicsBody(const mat4& Transform, const PhysicsBodyConfig& BodyConfig)
{
    PhysicsBodyHandle newHandle;

    RigidBody newBody;
    newBody.FPosition = Transform.Column(3).xyz();
    newBody.FMass = BodyConfig.FMass;

    FRigidbodies.emplace_back(newBody);

    newHandle.FId = FHandleGenerator.GetNextId();
    return newHandle;
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

    RigidBody& body = FRigidbodies[Handle.FId];
    body.FPosition = Position;
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

    RigidBody& body = FRigidbodies[Handle.FId];
    body.FPosition = Velocity;
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

    RigidBody& body = FRigidbodies[Handle.FId];

    if (bTreatAsAcceleration)
    {
        body.FAcceleration += Force;
    }
    else
    {
        body.FAcceleration += Force / body.FMass;
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

    RigidBody& body = FRigidbodies[Handle.FId];
    if (bTreatAsVelocityChange)
    {
        body.FVelocity += Impulse;
    }
    else
    {
        body.FVelocity += Impulse / body.FMass;
    }
    return false;
}

} // namespace Physics
} // namespace ECSEngine
