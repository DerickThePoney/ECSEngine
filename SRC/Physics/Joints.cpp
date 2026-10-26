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

void BallJoint::Initialize(const PhysicsBodyHandle& BodyA, const PhysicsBodyHandle& BodyB, const BallJointConfig& Config)
{
    FBodyA = BodyA;
    FBodyB = BodyB;
    FConfig = Config;

    FPointImpulse = vec3();
    bIsland = false;
}

void BallJoint::Destroy()
{
    FBodyA = PhysicsBodyHandle{ };
    FBodyB = PhysicsBodyHandle{ };
}

void ComputeConstraintForI(float& M_effi, float& biasi, vec3 ei, float Ci, RigidBody* bodyA, RigidBody* bodyB, const BallJointState& State, float parDT)
{
    vec3 rai = Cross(State.rA, ei);
    vec3 rbi = Cross(State.rB, ei);
    float Ki = bodyA->FInvMass + bodyB->FInvMass + Dot(rai, bodyA->FInverseInertiaTensorWorld * rai) + Dot(rbi, bodyB->FInverseInertiaTensorWorld * rbi);
    M_effi = (abs(Ki) < 1.e-3f) ? 0 : 1 / Ki;
    biasi = (abs(Ci) < LINEAR_SLOP) ? 0 : (BAUMGARTE / parDT) * Ci;
}

void BallJoint::PreSolve(Island* parIsland, const float parDT)
{
    RigidBody* bodyA = PhysicsEngine::Instance().GetRigidBody(FBodyA);
    RigidBody* bodyB = PhysicsEngine::Instance().GetRigidBody(FBodyB);
    AssertRelease(bodyA && bodyB);

    FState.xA = (bodyA->GetTransform() * vec4::MakeHomogeneousPositionVec4(FConfig.FCommon.FAnchorALocal)).xyz();
    FState.xB = (bodyB->GetTransform() * vec4::MakeHomogeneousPositionVec4(FConfig.FCommon.FAnchorBLocal)).xyz();

    FState.rA = FState.xA - bodyA->FCenterOfMassWorld;
    FState.rB = FState.xB - bodyB->FCenterOfMassWorld;

    FState.C = FState.xB - FState.xA;

    {
        ComputeConstraintForI(FState.M_eff.x, FState.bias.x, vec3(1.f, 0.f, 0.f), FState.C.x, bodyA, bodyB, FState, parDT);
        ComputeConstraintForI(FState.M_eff.y, FState.bias.y, vec3(0.f, 1.f, 0.f), FState.C.y, bodyA, bodyB, FState, parDT);
        ComputeConstraintForI(FState.M_eff.z, FState.bias.z, vec3(0.f, 0.f, 1.f), FState.C.z, bodyA, bodyB, FState, parDT);
    }

    vec3 P = FPointImpulse;
    vec3& vA = parIsland->FVelocities[bodyA->FIslandIndex].FLinearVelocity;
    vec3& wA = parIsland->FVelocities[bodyA->FIslandIndex].FRotationVelocity;
    vec3& vB = parIsland->FVelocities[bodyB->FIslandIndex].FLinearVelocity;
    vec3& wB = parIsland->FVelocities[bodyB->FIslandIndex].FRotationVelocity;
    vA -= P * bodyA->FInvMass;
    wA -= bodyA->FInverseInertiaTensorWorld * Cross(FState.rA, P);
    vB += P * bodyB->FInvMass;
    wB += bodyB->FInverseInertiaTensorWorld * Cross(FState.rB, P);
}

void ComputeImpulseForI(float& PointImpulseI, float& Pi, float v_reli, float M_effi, float biasi)
{
    float Jvi = v_reli;
    float dli = -M_effi * (Jvi + biasi);
    PointImpulseI += dli;
    Pi = dli;
}

void BallJoint::Solve(Island* parIsland, const float parDT)
{
    RigidBody* bodyA = PhysicsEngine::Instance().GetRigidBody(FBodyA);
    RigidBody* bodyB = PhysicsEngine::Instance().GetRigidBody(FBodyB);
    AssertRelease(bodyA && bodyB);

    vec3& vA = parIsland->FVelocities[bodyA->FIslandIndex].FLinearVelocity;
    vec3& wA = parIsland->FVelocities[bodyA->FIslandIndex].FRotationVelocity;
    vec3& vB = parIsland->FVelocities[bodyB->FIslandIndex].FLinearVelocity;
    vec3& wB = parIsland->FVelocities[bodyB->FIslandIndex].FRotationVelocity;

    vec3 v_rel = (vB + Cross(wB, FState.rB)) - (vA + Cross(wA, FState.rA));
    vec3 P;

    {
        ComputeImpulseForI(FPointImpulse.x, P.x, v_rel.x, FState.M_eff.x, FState.bias.x);
        ComputeImpulseForI(FPointImpulse.y, P.y, v_rel.y, FState.M_eff.y, FState.bias.y);
        ComputeImpulseForI(FPointImpulse.z, P.z, v_rel.z, FState.M_eff.z, FState.bias.z);
    }

    vA -= P * bodyA->FInvMass;
    wA -= bodyA->FInverseInertiaTensorWorld * Cross(FState.rA, P);
    vB += P * bodyB->FInvMass;
    wB += bodyB->FInverseInertiaTensorWorld * Cross(FState.rB, P);
}

} // namespace Physics
} // namespace ECSEngine
