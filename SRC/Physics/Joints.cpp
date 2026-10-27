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
static constexpr float ANGULAR_SLOP = 2.f * Pi() / 180.f; // ~2 deg

void JointBase::Initialize(const PhysicsBodyHandle& BodyA, const PhysicsBodyHandle& BodyB)
{
    FBodyA = BodyA;
    FBodyB = BodyB;
    bIsland = false;
}

void JointBase::Destroy()
{
    FBodyA = PhysicsBodyHandle{ };
    FBodyB = PhysicsBodyHandle{ };
}

void DistanceJoint::Initialize(const PhysicsBodyHandle& BodyA, const PhysicsBodyHandle& BodyB, const DistanceJointConfig& Config)
{
    JointBase::Initialize(BodyA, BodyB);
    FConfig = Config;
    FImpulse = 0.f;
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
    JointBase::Initialize(BodyA, BodyB);
    FConfig = Config;

    FPointImpulse = vec3();
}

void ComputeConstraintForI(float& M_effi, float& biasi, vec3 ei, float Ci, RigidBody* bodyA, RigidBody* bodyB, const vec3& rA, const vec3& rB, float parDT)
{
    vec3 rai = Cross(rA, ei);
    vec3 rbi = Cross(rB, ei);
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
        ComputeConstraintForI(FState.M_eff.x, FState.bias.x, vec3(1.f, 0.f, 0.f), FState.C.x, bodyA, bodyB, FState.rA, FState.rB, parDT);
        ComputeConstraintForI(FState.M_eff.y, FState.bias.y, vec3(0.f, 1.f, 0.f), FState.C.y, bodyA, bodyB, FState.rA, FState.rB, parDT);
        ComputeConstraintForI(FState.M_eff.z, FState.bias.z, vec3(0.f, 0.f, 1.f), FState.C.z, bodyA, bodyB, FState.rA, FState.rB, parDT);
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

void HingeJoint::Initialize(const PhysicsBodyHandle& BodyA, const PhysicsBodyHandle& BodyB, const HingeJointConfig& Config)
{
    JointBase::Initialize(BodyA, BodyB);

    FConfig = Config;

    RigidBody* bodyA = PhysicsEngine::Instance().GetRigidBody(BodyA);
    RigidBody* bodyB = PhysicsEngine::Instance().GetRigidBody(BodyB);
    AssertRelease(bodyA && bodyB);

    const mat4 RA = mat4(bodyA->FOrientation); // or rotation from GetTransform()
    const mat4 RB = mat4(bodyB->FOrientation);

    // If you only have mat4 poses:
    // const mat4 TA = bodyA->GetTransform();
    // rotate dirs with MakeHomogeneousDirectionVec4
    // 1) Axis on A (authored), world at bind
    FAxisALocal = Normalize(Config.FAxisALocal);
    const vec3 axisW = Normalize((RA * vec4::MakeHomogeneousDirectionVec4(FAxisALocal)).xyz());

    // 2) Same world axis expressed in B → AxisBLocal
    //    (inverse rotate: R^T * v for orthonormal R)
    const mat4 RBinv = Transpose(RB); // if RB is pure rotation
    FAxisBLocal = Normalize((RBinv * vec4::MakeHomogeneousDirectionVec4(axisW)).xyz());

    // 3) Reference “zero” mark ⊥ axis (for limits / angle)
    //    Any unit vector perpendicular to axisW:
    vec3 t = (abs(axisW.x) < 0.9f) ? vec3(1, 0, 0) : vec3(0, 1, 0);

    const vec3 refW = Normalize(Cross(axisW, t));
    FRefALocal = Normalize((Transpose(RA) * vec4::MakeHomogeneousDirectionVec4(refW)).xyz());
    FRefBLocal = Normalize((RBinv * vec4::MakeHomogeneousDirectionVec4(refW)).xyz());

    // 4) Limits relative to this bind pose
    FReferenceAngle = 0.f;
    FPrevAngle = 0.f;

    // 5) Warm-start / limit state
    FPointImpulse = vec3();
    FAxisImpulse[0] = FAxisImpulse[1] = 0.f;
    FLimitImpulse = 0.f;
    FLimitState = EHingeLimitState::Inactive;

    // Anchors stay in FConfig.FCommon (same as ball) — no bake needed

    // Set up motor data
    FEnableMotor = FConfig.FEnableMotor;
    FMotorSpeed = FConfig.FMotorSpeed;
    FMaxMotorTorque = FConfig.FMaxMotorTorque;

    FUseTargetAngle = FConfig.FUseTargetAngle;
    FTargetAngle = FConfig.FTargetAngle;
    FMotorStiffness = FConfig.FMotorStiffness;

    FMotorImpulse = 0.f;
}

static void ComputeBasis(const vec3& a, vec3& b, vec3& c)
{
    // same idea as ContactManager::ComputeBasis
    if (fabsf(a.x) >= 0.57735027f)
        b = vec3(a.y, -a.x, 0.f);
    else
        b = vec3(0.f, a.z, -a.y);
    b = Normalize(b);
    c = Cross(a, b);
}
static float UnwrapAngle(float a, float ref)
{
    while (a - ref > Pi())
        a -= 2.f * Pi();
    while (a - ref < -Pi())
        a += 2.f * Pi();
    return a;
}

void HingeJoint::PreSolve(Island* parIsland, const float parDT)
{
    RigidBody* bodyA = PhysicsEngine::Instance().GetRigidBody(FBodyA);
    RigidBody* bodyB = PhysicsEngine::Instance().GetRigidBody(FBodyB);
    AssertRelease(bodyA && bodyB);

    vec3& vA = parIsland->FVelocities[bodyA->FIslandIndex].FLinearVelocity;
    vec3& wA = parIsland->FVelocities[bodyA->FIslandIndex].FRotationVelocity;
    vec3& vB = parIsland->FVelocities[bodyB->FIslandIndex].FLinearVelocity;
    vec3& wB = parIsland->FVelocities[bodyB->FIslandIndex].FRotationVelocity;

    const mat4 TA = bodyA->GetTransform();
    const mat4 TB = bodyB->GetTransform();

    // ── Point-to-point (ball) ─────────────────────────────────
    FState.xA = (TA * vec4::MakeHomogeneousPositionVec4(FConfig.FCommon.FAnchorALocal)).xyz();
    FState.xB = (TB * vec4::MakeHomogeneousPositionVec4(FConfig.FCommon.FAnchorBLocal)).xyz();
    FState.rA = FState.xA - bodyA->FCenterOfMassWorld;
    FState.rB = FState.xB - bodyB->FCenterOfMassWorld;
    FState.C = FState.xB - FState.xA;

    // reuse ball helper
    ComputeConstraintForI(FState.M_eff.x, FState.bias.x, vec3(1, 0, 0), FState.C.x, bodyA, bodyB, FState.rA, FState.rB, parDT);
    ComputeConstraintForI(FState.M_eff.y, FState.bias.y, vec3(0, 1, 0), FState.C.y, bodyA, bodyB, FState.rA, FState.rB, parDT);
    ComputeConstraintForI(FState.M_eff.z, FState.bias.z, vec3(0, 0, 1), FState.C.z, bodyA, bodyB, FState.rA, FState.rB, parDT);

    {
        const vec3 P = FPointImpulse;
        vA -= P * bodyA->FInvMass;
        wA -= bodyA->FInverseInertiaTensorWorld * Cross(FState.rA, P);
        vB += P * bodyB->FInvMass;
        wB += bodyB->FInverseInertiaTensorWorld * Cross(FState.rB, P);
    }

    // ── Axis alignment ────────────────────────────────────────
    FState.aA = Normalize((TA * vec4::MakeHomogeneousDirectionVec4(FAxisALocal)).xyz());
    FState.aB = Normalize((TB * vec4::MakeHomogeneousDirectionVec4(FAxisBLocal)).xyz());
    ComputeBasis(FState.aA, FState.t1, FState.t2);
    FState.axisC[0] = Dot(FState.t1, FState.aB);
    FState.axisC[1] = Dot(FState.t2, FState.aB);
    FState.u1 = Cross(FState.aB, FState.t1);
    FState.u2 = Cross(FState.aB, FState.t2);

    for (int k = 0; k < 2; ++k)
    {
        const vec3& u = (k == 0) ? FState.u1 : FState.u2;
        const float K = Dot(u, bodyA->FInverseInertiaTensorWorld * u) + Dot(u, bodyB->FInverseInertiaTensorWorld * u);
        FState.axisM_eff[k] = (fabsf(K) < 1e-3f) ? 0.f : 1.f / K;
        FState.axisBias[k] = (fabsf(FState.axisC[k]) < ANGULAR_SLOP) ? 0.f : (BAUMGARTE / parDT) * FState.axisC[k];
    }

    {
        const vec3 tau = FState.u1 * FAxisImpulse[0] + FState.u2 * FAxisImpulse[1];
        wA -= bodyA->FInverseInertiaTensorWorld * tau;
        wB += bodyB->FInverseInertiaTensorWorld * tau;
    }

    // ── Limit ─────────────────────────────────────────────────
    FLimitState = EHingeLimitState::Inactive;
    FState.limitM_eff = 0.f;
    FState.limitBias = 0.f;
    if (FConfig.FEnableLimit)
    {
        const vec3 refA = (TA * vec4::MakeHomogeneousDirectionVec4(FRefALocal)).xyz();
        const vec3 refB = (TB * vec4::MakeHomogeneousDirectionVec4(FRefBLocal)).xyz();
        const float thetaRaw = atan2f(Dot(FState.aA, Cross(refA, refB)), Dot(refA, refB));
        FState.theta = UnwrapAngle(thetaRaw - FReferenceAngle, FPrevAngle);
        FPrevAngle = FState.theta;
        float C = 0.f;

        if (FState.theta < FConfig.FLowerAngle - ANGULAR_SLOP)
        {
            FLimitState = EHingeLimitState::AtLower;
            C = FState.theta - FConfig.FLowerAngle; // negative
        }
        else if (FState.theta > FConfig.FUpperAngle + ANGULAR_SLOP)
        {
            FLimitState = EHingeLimitState::AtUpper;
            C = FState.theta - FConfig.FUpperAngle; // positive
        }
        else
        {
            FLimitImpulse = 0.f;
        }

        if (FLimitState != EHingeLimitState::Inactive)
        {
            const float K = Dot(FState.aA, bodyA->FInverseInertiaTensorWorld * FState.aA) + Dot(FState.aA, bodyB->FInverseInertiaTensorWorld * FState.aA);
            FState.limitM_eff = (fabsf(K) < 1e-3f) ? 0.f : 1.f / K;
            FState.limitBias = 0.1f * (BAUMGARTE / parDT) * C;
            const vec3 axis = (FLimitState == EHingeLimitState::AtLower) ? FState.aA : Invert(FState.aA);
            const vec3 tau = axis * FLimitImpulse;
            wA -= bodyA->FInverseInertiaTensorWorld * tau;
            wB += bodyB->FInverseInertiaTensorWorld * tau;
        }
    }
}

void HingeJoint::Solve(Island* parIsland, const float parDT)
{
    RigidBody* bodyA = PhysicsEngine::Instance().GetRigidBody(FBodyA);
    RigidBody* bodyB = PhysicsEngine::Instance().GetRigidBody(FBodyB);
    AssertRelease(bodyA && bodyB);

    vec3& vA = parIsland->FVelocities[bodyA->FIslandIndex].FLinearVelocity;
    vec3& wA = parIsland->FVelocities[bodyA->FIslandIndex].FRotationVelocity;
    vec3& vB = parIsland->FVelocities[bodyB->FIslandIndex].FLinearVelocity;
    vec3& wB = parIsland->FVelocities[bodyB->FIslandIndex].FRotationVelocity;

    // ── Point ─────────────────────────────────────────────────
    {
        const vec3 v_rel = (vB + Cross(wB, FState.rB)) - (vA + Cross(wA, FState.rA));
        vec3 P;
        ComputeImpulseForI(FPointImpulse.x, P.x, v_rel.x, FState.M_eff.x, FState.bias.x);
        ComputeImpulseForI(FPointImpulse.y, P.y, v_rel.y, FState.M_eff.y, FState.bias.y);
        ComputeImpulseForI(FPointImpulse.z, P.z, v_rel.z, FState.M_eff.z, FState.bias.z);
        vA -= P * bodyA->FInvMass;
        wA -= bodyA->FInverseInertiaTensorWorld * Cross(FState.rA, P);
        vB += P * bodyB->FInvMass;
        wB += bodyB->FInverseInertiaTensorWorld * Cross(FState.rB, P);
    }

    // ── Axis alignment ────────────────────────────────────────
    for (int k = 0; k < 2; ++k)
    {
        const vec3& u = (k == 0) ? FState.u1 : FState.u2;
        const float Jv = Dot(u, wB - wA);
        const float dLambda = -FState.axisM_eff[k] * (Jv + FState.axisBias[k]);
        FAxisImpulse[k] += dLambda;
        const vec3 tau = u * dLambda;
        wA -= bodyA->FInverseInertiaTensorWorld * tau;
        wB += bodyB->FInverseInertiaTensorWorld * tau;
    }

    // ── Limit ─────────────────────────────────────────────────
    if (FLimitState != EHingeLimitState::Inactive)
    {
        // Jv = aA·(ωB-ωA); C = θ-bound.
        // AtLower (C<0): need to increase θ → clamp λ >= 0 with τ = +λ aA on B
        // AtUpper (C>0): need to decrease θ → store λ >= 0 but apply τ = -λ aA
        const vec3 axis = (FLimitState == EHingeLimitState::AtLower) ? FState.aA : Invert(FState.aA);
        // axis points in the direction that pushes back into the allowed range
        const float Jv = Dot(axis, wB - wA);
        float dLambda = -FState.limitM_eff * (Jv + FState.limitBias);
        const float old = FLimitImpulse;
        FLimitImpulse = Max(old + dLambda, 0.f); // unilateral
        dLambda = FLimitImpulse - old;
        const vec3 tau = axis * dLambda;
        wA -= bodyA->FInverseInertiaTensorWorld * tau;
        wB += bodyB->FInverseInertiaTensorWorld * tau;
    }
}

} // namespace Physics
} // namespace ECSEngine
