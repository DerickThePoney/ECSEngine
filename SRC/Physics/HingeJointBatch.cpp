#include "stdafx.h"

#include "HingeJointBatch.h"

#include "Island.h"
#include "JointBatchHelpers.h"
#include "PhysicsEngine.h"
#include "RigidBody.h"

namespace ECSEngine
{
namespace Physics
{

IMPLEMENT_POOL_ALLOCATED(HingeJointState);

void HingeJointBatch::Initialize(Island* /*island*/)
{
    FStates.clear();
    FStates.reserve(FJoints.size());

    PhysicsEngine& Engine = PhysicsEngine::Instance();
    foreachitem(J, FJoints)
    {
        RigidBody* bodyA = Engine.GetRigidBody(J->FBodyA);
        RigidBody* bodyB = Engine.GetRigidBody(J->FBodyB);
        AssertRelease(bodyA && bodyB);

        HingeJointState& S = FStates.emplace_back();
        S.Joint = J;
        S.IndexA = bodyA->FIslandIndex;
        S.IndexB = bodyB->FIslandIndex;
        S.MA = bodyA->FInvMass;
        S.MB = bodyB->FInvMass;
        S.IA = bodyA->FInverseInertiaTensorWorld;
        S.IB = bodyB->FInverseInertiaTensorWorld;
        S.PointImpulse = J->FPointImpulse;
        S.AxisImpulse[0] = J->FAxisImpulse[0];
        S.AxisImpulse[1] = J->FAxisImpulse[1];
        S.LimitImpulse = J->FLimitImpulse;
        S.MotorImpulse = J->FMotorImpulse;
        S.PrevAngle = J->FPrevAngle;
        S.LimitState = EHingeLimitState::Inactive;
    }
}

void HingeJointBatch::PreSolve(Island* island, float dt)
{
    PhysicsEngine& Engine = PhysicsEngine::Instance();
    foreachitem(S, FStates)
    {
        HingeJoint* J = S.Joint;
        RigidBody* bodyA = Engine.GetRigidBody(J->FBodyA);
        RigidBody* bodyB = Engine.GetRigidBody(J->FBodyB);
        AssertRelease(bodyA && bodyB);

        vec3& vA = island->FVelocities[S.IndexA].FLinearVelocity;
        vec3& wA = island->FVelocities[S.IndexA].FRotationVelocity;
        vec3& vB = island->FVelocities[S.IndexB].FLinearVelocity;
        vec3& wB = island->FVelocities[S.IndexB].FRotationVelocity;

        const mat4 TA = bodyA->GetTransform();
        const mat4 TB = bodyB->GetTransform();

        S.xA = (TA * vec4::MakeHomogeneousPositionVec4(J->FConfig.FCommon.FAnchorALocal)).xyz();
        S.xB = (TB * vec4::MakeHomogeneousPositionVec4(J->FConfig.FCommon.FAnchorBLocal)).xyz();
        S.rA = S.xA - bodyA->FCenterOfMassWorld;
        S.rB = S.xB - bodyB->FCenterOfMassWorld;
        S.C = S.xB - S.xA;

        ComputeJointConstraintForI(S.M_eff.x, S.bias.x, vec3(1, 0, 0), S.C.x, S.MA, S.MB, S.IA, S.IB, S.rA, S.rB, dt);
        ComputeJointConstraintForI(S.M_eff.y, S.bias.y, vec3(0, 1, 0), S.C.y, S.MA, S.MB, S.IA, S.IB, S.rA, S.rB, dt);
        ComputeJointConstraintForI(S.M_eff.z, S.bias.z, vec3(0, 0, 1), S.C.z, S.MA, S.MB, S.IA, S.IB, S.rA, S.rB, dt);

        {
            const vec3 P = S.PointImpulse;
            vA -= P * S.MA;
            wA -= S.IA * Cross(S.rA, P);
            vB += P * S.MB;
            wB += S.IB * Cross(S.rB, P);
        }

        S.aA = Normalize((TA * vec4::MakeHomogeneousDirectionVec4(J->FAxisALocal)).xyz());
        S.aB = Normalize((TB * vec4::MakeHomogeneousDirectionVec4(J->FAxisBLocal)).xyz());
        JointComputeBasis(S.aA, S.t1, S.t2);
        S.axisC[0] = Dot(S.t1, S.aB);
        S.axisC[1] = Dot(S.t2, S.aB);
        S.u1 = Cross(S.aB, S.t1);
        S.u2 = Cross(S.aB, S.t2);

        for (int k = 0; k < 2; ++k)
        {
            const vec3& u = (k == 0) ? S.u1 : S.u2;
            const float K = Dot(u, S.IA * u) + Dot(u, S.IB * u);
            S.axisM_eff[k] = (fabsf(K) < 1e-3f) ? 0.f : 1.f / K;
            S.axisBias[k] = (fabsf(S.axisC[k]) < JOINT_ANGULAR_SLOP) ? 0.f : (JOINT_BAUMGARTE / dt) * S.axisC[k];
        }

        {
            const vec3 tau = S.u1 * S.AxisImpulse[0] + S.u2 * S.AxisImpulse[1];
            wA -= S.IA * tau;
            wB += S.IB * tau;
        }

        const bool needTheta = J->FConfig.FEnableLimit || (J->FEnableMotor && J->FUseTargetAngle);
        if (needTheta)
        {
            const vec3 refA = (TA * vec4::MakeHomogeneousDirectionVec4(J->FRefALocal)).xyz();
            const vec3 refB = (TB * vec4::MakeHomogeneousDirectionVec4(J->FRefBLocal)).xyz();
            const float thetaRaw = atan2f(Dot(S.aA, Cross(refA, refB)), Dot(refA, refB));
            S.theta = JointUnwrapAngle(thetaRaw - J->FReferenceAngle, S.PrevAngle);
            S.PrevAngle = S.theta;
        }

        S.LimitState = EHingeLimitState::Inactive;
        S.limitM_eff = 0.f;
        S.limitBias = 0.f;
        if (J->FConfig.FEnableLimit)
        {
            float C = 0.f;
            if (S.theta < J->FConfig.FLowerAngle - JOINT_ANGULAR_SLOP)
            {
                S.LimitState = EHingeLimitState::AtLower;
                C = S.theta - J->FConfig.FLowerAngle;
            }
            else if (S.theta > J->FConfig.FUpperAngle + JOINT_ANGULAR_SLOP)
            {
                S.LimitState = EHingeLimitState::AtUpper;
                C = S.theta - J->FConfig.FUpperAngle;
            }
            else
            {
                S.LimitImpulse = 0.f;
            }

            if (S.LimitState != EHingeLimitState::Inactive)
            {
                const float K = Dot(S.aA, S.IA * S.aA) + Dot(S.aA, S.IB * S.aA);
                S.limitM_eff = (fabsf(K) < 1e-3f) ? 0.f : 1.f / K;
                S.limitBias = 0.1f * (JOINT_BAUMGARTE / dt) * C;
                const vec3 axis = (S.LimitState == EHingeLimitState::AtLower) ? S.aA : Invert(S.aA);
                const vec3 tau = axis * S.LimitImpulse;
                wA -= S.IA * tau;
                wB += S.IB * tau;
            }
        }

        S.motorM_eff = 0.f;
        S.motorSpeed = 0.f;
        S.maxMotorImpulse = 0.f;
        const bool limitLocked = J->FConfig.FEnableLimit && (J->FConfig.FUpperAngle - J->FConfig.FLowerAngle) <= JOINT_ANGULAR_SLOP;
        if (J->FEnableMotor && !limitLocked)
        {
            S.motorSpeed = J->FMotorSpeed;
            if (J->FUseTargetAngle)
                S.motorSpeed = J->FMotorStiffness * (J->FTargetAngle - S.theta);

            if (S.LimitState == EHingeLimitState::AtLower && S.motorSpeed < 0.f)
                S.motorSpeed = 0.f;
            if (S.LimitState == EHingeLimitState::AtUpper && S.motorSpeed > 0.f)
                S.motorSpeed = 0.f;

            const float K = Dot(S.aA, S.IA * S.aA) + Dot(S.aA, S.IB * S.aA);
            S.motorM_eff = (fabsf(K) < 1e-3f) ? 0.f : 1.f / K;
            S.maxMotorImpulse = J->FMaxMotorTorque * dt;

            const vec3 tau = S.aA * S.MotorImpulse;
            wA -= S.IA * tau;
            wB += S.IB * tau;
        }
        else
        {
            S.MotorImpulse = 0.f;
        }
    }
}

void HingeJointBatch::Solve(Island* island)
{
    foreachitem(S, FStates)
    {
        vec3& vA = island->FVelocities[S.IndexA].FLinearVelocity;
        vec3& wA = island->FVelocities[S.IndexA].FRotationVelocity;
        vec3& vB = island->FVelocities[S.IndexB].FLinearVelocity;
        vec3& wB = island->FVelocities[S.IndexB].FRotationVelocity;

        {
            const vec3 v_rel = (vB + Cross(wB, S.rB)) - (vA + Cross(wA, S.rA));
            vec3 P;
            ComputeJointImpulseForI(S.PointImpulse.x, P.x, v_rel.x, S.M_eff.x, S.bias.x);
            ComputeJointImpulseForI(S.PointImpulse.y, P.y, v_rel.y, S.M_eff.y, S.bias.y);
            ComputeJointImpulseForI(S.PointImpulse.z, P.z, v_rel.z, S.M_eff.z, S.bias.z);
            vA -= P * S.MA;
            wA -= S.IA * Cross(S.rA, P);
            vB += P * S.MB;
            wB += S.IB * Cross(S.rB, P);
        }

        for (int k = 0; k < 2; ++k)
        {
            const vec3& u = (k == 0) ? S.u1 : S.u2;
            const float Jv = Dot(u, wB - wA);
            const float dLambda = -S.axisM_eff[k] * (Jv + S.axisBias[k]);
            S.AxisImpulse[k] += dLambda;
            const vec3 tau = u * dLambda;
            wA -= S.IA * tau;
            wB += S.IB * tau;
        }

        if (S.Joint->FEnableMotor && S.motorM_eff > 0.f)
        {
            const float Jv = Dot(S.aA, wB - wA);
            const float Cdot = Jv - S.motorSpeed;
            float dLambda = -S.motorM_eff * Cdot;
            const float old = S.MotorImpulse;
            S.MotorImpulse = Clamp(old + dLambda, -S.maxMotorImpulse, S.maxMotorImpulse);
            dLambda = S.MotorImpulse - old;
            const vec3 tau = S.aA * dLambda;
            wA -= S.IA * tau;
            wB += S.IB * tau;
        }

        if (S.LimitState != EHingeLimitState::Inactive)
        {
            const vec3 axis = (S.LimitState == EHingeLimitState::AtLower) ? S.aA : Invert(S.aA);
            const float Jv = Dot(axis, wB - wA);
            float dLambda = -S.limitM_eff * (Jv + S.limitBias);
            const float old = S.LimitImpulse;
            S.LimitImpulse = Max(old + dLambda, 0.f);
            dLambda = S.LimitImpulse - old;
            const vec3 tau = axis * dLambda;
            wA -= S.IA * tau;
            wB += S.IB * tau;
        }
    }
}

void HingeJointBatch::Shutdown()
{
    foreachitem(S, FStates)
    {
        S.Joint->FPointImpulse = S.PointImpulse;
        S.Joint->FAxisImpulse[0] = S.AxisImpulse[0];
        S.Joint->FAxisImpulse[1] = S.AxisImpulse[1];
        S.Joint->FLimitImpulse = S.LimitImpulse;
        S.Joint->FMotorImpulse = S.MotorImpulse;
        S.Joint->FPrevAngle = S.PrevAngle;
        S.Joint->FLimitState = S.LimitState;
    }
}

} // namespace Physics
} // namespace ECSEngine
