#include "stdafx.h"

#include "Island.h"

#include "Contact.h"
#include "ContactSolver.h"
#include "ContactState.h"
#include "PhysicsEngine.h"
#include "PhysicsEngineConfiguration.h"
#include "RigidBody.h"

namespace ECSEngine
{
namespace Physics
{

void Island::Reserve(u32 BodyCount, u32 ContactCount)
{
    FBodies.reserve(BodyCount);
    FVelocities.reserve(BodyCount);
    FContacts.reserve(ContactCount);
    FContactStates.reserve(ContactCount);
}

void Island::Reset()
{
    FBodies.clear();
    FVelocities.clear();
    FContacts.clear();
    FContactStates.clear();
}

void Island::Add(RigidBody* BodyToAdd)
{
    BodyToAdd->FIslandIndex = FBodies.size();
    FBodies.push_back(BodyToAdd);
    FVelocities.emplace_back(VelocityState{ BodyToAdd->FVelocity, BodyToAdd->FRotationVelocity });
}

void Island::Add(Contact* ContactToAdd)
{
    FContacts.push_back(ContactToAdd);
}

void Island::Initialise()
{
    foreachitemconst(C, FContacts)
    {
        ContactState& CS = FContactStates.emplace_back();
        RigidBody* bodyA = PhysicsEngine::Instance().GetRigidBody(C->FFirstBody);
        RigidBody* bodyB = PhysicsEngine::Instance().GetRigidBody(C->FSecondBody);
        AssertRelease(bodyA && bodyB);
        CS.CenterA = bodyA->FCenterOfMassWorld;
        CS.CenterB = bodyB->FCenterOfMassWorld;
        CS.IA = bodyA->FInverseInertiaTensorWorld;
        CS.IB = bodyB->FInverseInertiaTensorWorld;
        CS.MA = bodyA->FInvMass;
        CS.MB = bodyB->FInvMass;

        // TODO: Restitution and friction
        CS.Friction = C->FFriction;
        CS.Restitution = C->FRestitution;

        CS.IndexA = bodyA->FIslandIndex;
        CS.IndexB = bodyB->FIslandIndex;

        // Contact copies
        CS.Normal = C->FContactNormal;
        CS.TangentVectors[0] = C->FContactTangents[0];
        CS.TangentVectors[1] = C->FContactTangents[1];
        i32 MaxContactsToWrite = Min(C->FManifold.FContactPoints.size(), 8);
        CS.NumberOfContacts = MaxContactsToWrite;

        forrange(i, 0, MaxContactsToWrite)
        {
            ContactPointState& CPS = CS.ContactPoints[i];
            ContactPoint& CP = C->FManifold.FContactPoints[i];
            CPS.CtoA = CP.FPosition - CS.CenterA;
            CPS.CtoB = CP.FPosition - CS.CenterB;
            CPS.Penetration = CP.FPenetration;
            CPS.NormalImpulse = CP.FNormalImpulse;
            CPS.TangentImpulses[0] = CP.FTangentImpulse[0];
            CPS.TangentImpulses[1] = CP.FTangentImpulse[1];
        }
    }
}

void Island::Solve(float parDeltaTime)
{
    PhysicsEngine& Engine = PhysicsEngine::Instance();
    const PhysicsEngineConfiguration& PhysicsConfig = Engine.Config();
    // Apply gravity and integrate forces
    forrange(i, 0, FBodies.size())
    {
        RigidBody* body = FBodies[i];
        AlwaysCheckedAssert(body != nullptr);
        if (body == nullptr)
        {
            continue;
        }

        if (body->FMoveabilityType == EPhysicsMoveability::PHYICS_ENABLED)
        {
            body->FVelocity += (body->FAccelerationDueToForces + body->FGravityScale * PhysicsConfig.FGravityValue * vec3(0.f, -1.f, 0.f)) * parDeltaTime;
            body->FRotationVelocity += body->FInverseInertiaTensorWorld * body->FTorque * parDeltaTime;

            // damping
            body->FVelocity *= 1.f / (1.f + parDeltaTime * body->FLinearDamping);
            body->FRotationVelocity *= 1.f / (1.f + parDeltaTime * body->FAngularDamping);

            body->FAccelerationDueToForces = vec3(0.f);
            body->FTorque = vec3(0.f);
        }

        FVelocities[i].FLinearVelocity = body->FVelocity;
        FVelocities[i].FRotationVelocity = body->FRotationVelocity;
    }

    // solve contact contraints
    {
        ContactSolver Solver;
        Solver.Initialise(this);
        Solver.PreSolve(parDeltaTime);

        forrange(i, 0, 10)
        {
            Solver.Solve();
        }

        Solver.Shutdown();
    }

    // solve and integrate and synchronize positions
    float MinSleepTimer = std::numeric_limits<float>::max();
    forrange(i, 0, FBodies.size())
    {
        RigidBody* body = FBodies[i];
        AlwaysCheckedAssert(body != nullptr);
        if (body == nullptr)
        {
            continue;
        }

        if (body->FMoveabilityType == EPhysicsMoveability::STATIC)
        {
            continue;
        }

        const VelocityState& Velocities = FVelocities[i];

        body->FVelocity = Velocities.FLinearVelocity;
        body->FRotationVelocity = Velocities.FRotationVelocity;

        const float sqrLinVel = Dot(body->FVelocity, body->FVelocity);
        const float sqrAngVel = Dot(body->FRotationVelocity, body->FRotationVelocity);
        static constexpr float linTol = 0.01f;
        static constexpr float angTol = 3.f * Pi() / 180.f;

        if (sqrLinVel > linTol || sqrAngVel > angTol)
        {
            const vec3 displacement = body->FVelocity * parDeltaTime;
            body->FCenterOfMassWorld += displacement;
            body->FOrientation = AddVectorToQuaternion(body->FOrientation, body->FRotationVelocity * parDeltaTime);
            vec3 oldPosition = body->FPosition;
            body->FPosition = body->FCenterOfMassWorld - (mat4(body->FOrientation) * vec4::MakeHomogeneousPositionVec4(body->FCenterOfMassLocal)).xyz();
            body->FSleepingTimer = 0.f;
            MinSleepTimer = 0.f;
            Engine.AddMovedBody(body, body->FPosition - oldPosition);
        }
        else
        {
            body->FSleepingTimer += parDeltaTime;
            MinSleepTimer = Min(body->FSleepingTimer, MinSleepTimer);
        }
    }

    // if the minsleep timer is lower than some tolerance, put everyone to sleep
    if (MinSleepTimer > 0.5f)
    {
        foreachitem(body, FBodies)
        {
            AlwaysCheckedAssert(body != nullptr);
            if (body == nullptr)
            {
                continue;
            }

            body->SetAwake(false);
        }
    }

    // set to sleep TODO
}
} // namespace Physics
} // namespace ECSEngine