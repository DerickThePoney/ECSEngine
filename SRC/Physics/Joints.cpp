#include "stdafx.h"

#include "Joints.h"

#include "Island.h"
#include "PhysicsEngine.h"

namespace ECSEngine
{
namespace Physics
{
static constexpr float BAUMGARTE = 0.3f;
static constexpr float LINEAR_SLOP = 0.005f;

void DistanceJoint::Initialize(const PhysicsBodyHandle& BodyA, const PhysicsBodyHandle& BodyB, const DistanceJointConfig& Config)
{
    FBodyA = BodyA;
    FBodyB = BodyB;
    FConfig = Config;

    FImpulse = 0;
    bIsland = false;
}

void DistanceJoint::Destroy()
{
    FBodyA = PhysicsBodyHandle{ };
    FBodyB = PhysicsBodyHandle{ };
}

void DistanceJoint::PreSolve(Island* parIsland, const float parDT)
{
    RigidBody* bodyA = PhysicsEngine::Instance().GetRigidBody(FBodyA);
    RigidBody* bodyB = PhysicsEngine::Instance().GetRigidBody(FBodyB);
    AssertRelease(bodyA && bodyB);

    FState.xA = (bodyA->GetTransform() * vec4::MakeHomogeneousPositionVec4(FConfig.FCommon.FAnchorALocal)).xyz();
    FState.xB = (bodyB->GetTransform() * vec4::MakeHomogeneousPositionVec4(FConfig.FCommon.FAnchorBLocal)).xyz();

    FState.rA = FState.xA - bodyA->FCenterOfMassWorld;
    FState.rB = FState.xB - bodyB->FCenterOfMassWorld;

    FState.d = FState.xB - FState.xA;
    float dl = Length(FState.d);
    FState.C = dl - FConfig.FLength;
    FState.n = (dl * dl < 1e-5f) ? vec3(1.f, 0.f, 0.f) : Normalize(FState.d);

    vec3 rA_n = Cross(FState.rA, FState.n);
    vec3 rB_n = Cross(FState.rB, FState.n);
    float K_n = bodyA->FInvMass + bodyB->FInvMass + Dot(rA_n, bodyA->FInverseInertiaTensorWorld * rA_n) + Dot(rB_n, bodyB->FInverseInertiaTensorWorld * rB_n);
    FState.M_eff = (abs(K_n) < 1e-3f) ? 0.f : 1.f / K_n;

    FState.bias = (abs(FState.C) < LINEAR_SLOP) ? 0.f : (BAUMGARTE / parDT) * FState.C;

    // WarmStart: Apply cached Impulse on the velocities
    vec3 P = FImpulse * FState.n;
    vec3& vA = parIsland->FVelocities[bodyA->FIslandIndex].FLinearVelocity;
    vec3& wA = parIsland->FVelocities[bodyA->FIslandIndex].FRotationVelocity;
    vec3& vB = parIsland->FVelocities[bodyB->FIslandIndex].FLinearVelocity;
    vec3& wB = parIsland->FVelocities[bodyB->FIslandIndex].FRotationVelocity;
    vA -= P * bodyA->FInvMass;
    wA -= bodyA->FInverseInertiaTensorWorld * Cross(FState.rA, P);
    vB += P * bodyB->FInvMass;
    wB += bodyB->FInverseInertiaTensorWorld * Cross(FState.rB, P);
}

void DistanceJoint::Solve(Island* parIsland, const float parDT)
{
    RigidBody* bodyA = PhysicsEngine::Instance().GetRigidBody(FBodyA);
    RigidBody* bodyB = PhysicsEngine::Instance().GetRigidBody(FBodyB);
    AssertRelease(bodyA && bodyB);

    vec3& vA = parIsland->FVelocities[bodyA->FIslandIndex].FLinearVelocity;
    vec3& wA = parIsland->FVelocities[bodyA->FIslandIndex].FRotationVelocity;
    vec3& vB = parIsland->FVelocities[bodyB->FIslandIndex].FLinearVelocity;
    vec3& wB = parIsland->FVelocities[bodyB->FIslandIndex].FRotationVelocity;

    vec3 v_rel = (vB + Cross(wB, FState.rB)) - (vA + Cross(wA, FState.rA));

    // Speed along side n
    float Jv = Dot(FState.n, v_rel);

    float DI = -FState.M_eff * (Jv + FState.bias);
    FImpulse += DI;
    vec3 P = DI * FState.n;

    vA -= P * bodyA->FInvMass;
    wA -= bodyA->FInverseInertiaTensorWorld * Cross(FState.rA, P);
    vB += P * bodyB->FInvMass;
    wB += bodyB->FInverseInertiaTensorWorld * Cross(FState.rB, P);
}
} // namespace Physics
} // namespace ECSEngine
