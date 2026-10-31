#include "stdafx.h"

#include "DistanceJointBatch.h"

#include "Island.h"
#include "JointBatchHelpers.h"
#include "PhysicsEngine.h"
#include "RigidBody.h"

namespace ECSEngine
{
namespace Physics
{

IMPLEMENT_POOL_ALLOCATED(DistanceJointState);

void DistanceJointBatch::Initialize(Island* /*island*/)
{
    FStates.clear();
    FStates.reserve(FJoints.size());

    PhysicsEngine& Engine = PhysicsEngine::Instance();
    foreachitem(J, FJoints)
    {
        RigidBody* bodyA = Engine.GetRigidBody(J->FBodyA);
        RigidBody* bodyB = Engine.GetRigidBody(J->FBodyB);
        AssertRelease(bodyA && bodyB);

        DistanceJointState& S = FStates.emplace_back();
        S.Joint = J;
        S.IndexA = bodyA->FIslandIndex;
        S.IndexB = bodyB->FIslandIndex;
        S.MA = bodyA->FInvMass;
        S.MB = bodyB->FInvMass;
        S.IA = bodyA->FInverseInertiaTensorWorld;
        S.IB = bodyB->FInverseInertiaTensorWorld;
        S.Impulse = J->FImpulse;
    }
}

void DistanceJointBatch::PreSolve(Island* island, float dt)
{
    PhysicsEngine& Engine = PhysicsEngine::Instance();
    foreachitem(S, FStates)
    {
        DistanceJoint* J = S.Joint;
        RigidBody* bodyA = Engine.GetRigidBody(J->FBodyA);
        RigidBody* bodyB = Engine.GetRigidBody(J->FBodyB);
        AssertRelease(bodyA && bodyB);

        S.xA = (bodyA->GetTransform() * vec4::MakeHomogeneousPositionVec4(J->FConfig.FCommon.FAnchorALocal)).xyz();
        S.xB = (bodyB->GetTransform() * vec4::MakeHomogeneousPositionVec4(J->FConfig.FCommon.FAnchorBLocal)).xyz();
        S.rA = S.xA - bodyA->FCenterOfMassWorld;
        S.rB = S.xB - bodyB->FCenterOfMassWorld;
        S.d = S.xB - S.xA;
        const float dl = Length(S.d);
        S.C = dl - J->FConfig.FLength;
        S.n = (dl * dl < 1e-5f) ? vec3(1.f, 0.f, 0.f) : Normalize(S.d);

        const vec3 rA_n = Cross(S.rA, S.n);
        const vec3 rB_n = Cross(S.rB, S.n);
        const float K_n = S.MA + S.MB + Dot(rA_n, S.IA * rA_n) + Dot(rB_n, S.IB * rB_n);
        S.M_eff = (abs(K_n) < 1e-3f) ? 0.f : 1.f / K_n;
        S.bias = (abs(S.C) < JOINT_LINEAR_SLOP) ? 0.f : (JOINT_BAUMGARTE / dt) * S.C;

        const vec3 P = S.Impulse * S.n;
        vec3& vA = island->FVelocities[S.IndexA].FLinearVelocity;
        vec3& wA = island->FVelocities[S.IndexA].FRotationVelocity;
        vec3& vB = island->FVelocities[S.IndexB].FLinearVelocity;
        vec3& wB = island->FVelocities[S.IndexB].FRotationVelocity;
        vA -= P * S.MA;
        wA -= S.IA * Cross(S.rA, P);
        vB += P * S.MB;
        wB += S.IB * Cross(S.rB, P);
    }
}

void DistanceJointBatch::Solve(Island* island)
{
    foreachitem(S, FStates)
    {
        vec3& vA = island->FVelocities[S.IndexA].FLinearVelocity;
        vec3& wA = island->FVelocities[S.IndexA].FRotationVelocity;
        vec3& vB = island->FVelocities[S.IndexB].FLinearVelocity;
        vec3& wB = island->FVelocities[S.IndexB].FRotationVelocity;

        const vec3 v_rel = (vB + Cross(wB, S.rB)) - (vA + Cross(wA, S.rA));
        const float Jv = Dot(S.n, v_rel);
        const float DI = -S.M_eff * (Jv + S.bias);
        S.Impulse += DI;
        const vec3 P = DI * S.n;

        vA -= P * S.MA;
        wA -= S.IA * Cross(S.rA, P);
        vB += P * S.MB;
        wB += S.IB * Cross(S.rB, P);
    }
}

void DistanceJointBatch::Shutdown()
{
    foreachitem(S, FStates)
    {
        S.Joint->FImpulse = S.Impulse;
    }
}

} // namespace Physics
} // namespace ECSEngine
