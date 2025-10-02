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
        CS.CenterA = bodyA->FPosition;
        CS.CenterB = bodyB->FPosition;
        CS.IA = bodyA->FInverseInertiaTensor;
        CS.IB = bodyB->FInverseInertiaTensor;
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
        CS.NumberOfContacts = C->FManifold.FContactPoints.size();

        forrange(i, 0, CS.NumberOfContacts)
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

        const vec3 displacement = body->FVelocity * parDeltaTime;
        body->FPosition += displacement;
        body->FOrientation = AddVectorToQuaternion(body->FOrientation, body->FRotationVelocity * parDeltaTime);

        Engine.AddMovedBody(body, displacement);
    }

    // set to sleep TODO
}
} // namespace Physics
} // namespace ECSEngine