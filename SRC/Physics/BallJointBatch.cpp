#include "stdafx.h"

#include "BallJointBatch.h"

#include "Island.h"
#include "JointBatchHelpers.h"
#include "PhysicsEngine.h"
#include "RigidBody.h"

namespace ECSEngine
{
namespace Physics
{

IMPLEMENT_POOL_ALLOCATED(BallJointState);

void BallJointBatch::Initialize(Island* /*island*/)
{
    FStates.clear();
    FStates.reserve(FJoints.size());

    PhysicsEngine& Engine = PhysicsEngine::Instance();
    foreachitem(J, FJoints)
    {
        RigidBody* bodyA = Engine.GetRigidBody(J->FBodyA);
        RigidBody* bodyB = Engine.GetRigidBody(J->FBodyB);
        AssertRelease(bodyA && bodyB);

        BallJointState& S = FStates.emplace_back();
        S.Joint = J;
        S.IndexA = bodyA->FIslandIndex;
        S.IndexB = bodyB->FIslandIndex;
        S.MA = bodyA->FInvMass;
        S.MB = bodyB->FInvMass;
        S.IA = bodyA->FInverseInertiaTensorWorld;
        S.IB = bodyB->FInverseInertiaTensorWorld;
        S.PointImpulse = J->FPointImpulse;
        S.MotorImpulse = J->FMotorImpulse;
        S.ConeLimitImpulse = J->FConeLimitImpulse;
    }
}

void BallJointBatch::PreSolve(Island* island, float dt)
{
    PhysicsEngine& Engine = PhysicsEngine::Instance();
    foreachitem(S, FStates)
    {
        BallJoint* J = S.Joint;
        RigidBody* bodyA = Engine.GetRigidBody(J->FBodyA);
        RigidBody* bodyB = Engine.GetRigidBody(J->FBodyB);
        AssertRelease(bodyA && bodyB);

        vec3& vA = island->FVelocities[S.IndexA].FLinearVelocity;
        vec3& wA = island->FVelocities[S.IndexA].FRotationVelocity;
        vec3& vB = island->FVelocities[S.IndexB].FLinearVelocity;
        vec3& wB = island->FVelocities[S.IndexB].FRotationVelocity;

        S.xA = (bodyA->GetTransform() * vec4::MakeHomogeneousPositionVec4(J->FConfig.FCommon.FAnchorALocal)).xyz();
        S.xB = (bodyB->GetTransform() * vec4::MakeHomogeneousPositionVec4(J->FConfig.FCommon.FAnchorBLocal)).xyz();
        S.rA = S.xA - bodyA->FCenterOfMassWorld;
        S.rB = S.xB - bodyB->FCenterOfMassWorld;
        S.C = S.xB - S.xA;

        ComputeJointConstraintForI(S.M_eff.x, S.bias.x, vec3(1.f, 0.f, 0.f), S.C.x, S.MA, S.MB, S.IA, S.IB, S.rA, S.rB, dt);
        ComputeJointConstraintForI(S.M_eff.y, S.bias.y, vec3(0.f, 1.f, 0.f), S.C.y, S.MA, S.MB, S.IA, S.IB, S.rA, S.rB, dt);
        ComputeJointConstraintForI(S.M_eff.z, S.bias.z, vec3(0.f, 0.f, 1.f), S.C.z, S.MA, S.MB, S.IA, S.IB, S.rA, S.rB, dt);

        {
            const vec3 P = S.PointImpulse;
            vA -= P * S.MA;
            wA -= S.IA * Cross(S.rA, P);
            vB += P * S.MB;
            wB += S.IB * Cross(S.rB, P);
        }

        const mat4 TA = bodyA->GetTransform();
        const mat4 TB = bodyB->GetTransform();

        S.coneLimitActive = false;
        S.coneM_eff = 0.f;
        S.coneBias = 0.f;
        S.coneAxis = vec3();
        S.coneAngle = 0.f;

        if (J->FConfig.FEnableConeLimit)
        {
            const vec3 aA = Normalize((TA * vec4::MakeHomogeneousDirectionVec4(J->FAxisALocal)).xyz());
            const vec3 aB = Normalize((TB * vec4::MakeHomogeneousDirectionVec4(J->FAxisBLocal)).xyz());
            const float cosTheta = Clamp(Dot(aA, aB), -1.f, 1.f);
            S.coneAngle = acosf(cosTheta);

            if (S.coneAngle > J->FConfig.FMaxConeAngle - JOINT_ANGULAR_SLOP)
            {
                vec3 u = Cross(aA, aB);
                const float uLen = Length(u);
                if (uLen > 1e-4f)
                {
                    u *= 1.f / uLen;
                    S.coneAxis = Invert(u);
                    S.coneLimitActive = true;

                    const float C = S.coneAngle - J->FConfig.FMaxConeAngle;
                    const float K = Dot(S.coneAxis, S.IA * S.coneAxis) + Dot(S.coneAxis, S.IB * S.coneAxis);
                    S.coneM_eff = (fabsf(K) < 1e-3f) ? 0.f : 1.f / K;
                    S.coneBias = (C < JOINT_ANGULAR_SLOP) ? 0.f : 0.1f * (JOINT_BAUMGARTE / dt) * C;
                }
                else
                {
                    S.ConeLimitImpulse = 0.f;
                }
            }
            else
            {
                S.ConeLimitImpulse = 0.f;
            }
        }

        S.motorM_eff = vec3();
        S.motorVelocityWorld = vec3();
        S.maxMotorImpulse = 0.f;

        if (J->FEnableMotor)
        {
            const mat4 RA = mat4(bodyA->FOrientation);
            S.e0 = Normalize((RA * vec4::MakeHomogeneousDirectionVec4(vec3(1, 0, 0))).xyz());
            S.e1 = Normalize((RA * vec4::MakeHomogeneousDirectionVec4(vec3(0, 1, 0))).xyz());
            S.e2 = Normalize((RA * vec4::MakeHomogeneousDirectionVec4(vec3(0, 0, 1))).xyz());

            vec3 motorVelLocal = J->FMotorVelocityLocal;
            if (J->FUseTargetOrientation)
            {
                const quat qRel = JointQuatConjugate(bodyA->FOrientation) * bodyB->FOrientation;
                const mat4 Rtarget = EulerAnglesXYZ(J->FTargetEulerAngles.x, J->FTargetEulerAngles.y, J->FTargetEulerAngles.z);
                const quat qTarget = quat::FromMat4(Rtarget);
                const quat qErr = JointQuatConjugate(qRel) * qTarget;
                motorVelLocal = JointQuatToRotVec(qErr) * J->FMotorStiffness;
            }

            S.motorVelocityWorld = S.e0 * motorVelLocal.x + S.e1 * motorVelLocal.y + S.e2 * motorVelLocal.z;

            if (S.coneLimitActive)
            {
                const vec3 u = Invert(S.coneAxis);
                const float outward = Dot(S.motorVelocityWorld, u);
                if (outward > 0.f)
                    S.motorVelocityWorld -= u * outward;
            }

            auto angularMeff = [&](const vec3& e) -> float
            {
                const float K = Dot(e, S.IA * e) + Dot(e, S.IB * e);
                return (fabsf(K) < 1e-3f) ? 0.f : 1.f / K;
            };
            S.motorM_eff.x = angularMeff(S.e0);
            S.motorM_eff.y = angularMeff(S.e1);
            S.motorM_eff.z = angularMeff(S.e2);
            S.maxMotorImpulse = J->FMaxMotorTorque * dt;

            const vec3 tau = S.e0 * S.MotorImpulse.x + S.e1 * S.MotorImpulse.y + S.e2 * S.MotorImpulse.z;
            wA -= S.IA * tau;
            wB += S.IB * tau;
        }
        else
        {
            S.MotorImpulse = vec3();
        }

        if (S.coneLimitActive)
        {
            const vec3 tau = S.coneAxis * S.ConeLimitImpulse;
            wA -= S.IA * tau;
            wB += S.IB * tau;
        }
    }
}

void BallJointBatch::Solve(Island* island)
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

        if (S.Joint->FEnableMotor)
        {
            auto solveMotorAxis = [&](float& impulseI, float M_effi, const vec3& e)
            {
                if (M_effi <= 0.f)
                    return;
                const float targetW = Dot(e, S.motorVelocityWorld);
                const float Jv = Dot(e, wB - wA);
                float dLambda = -M_effi * (Jv - targetW);
                const float old = impulseI;
                impulseI = Clamp(old + dLambda, -S.maxMotorImpulse, S.maxMotorImpulse);
                dLambda = impulseI - old;
                const vec3 tau = e * dLambda;
                wA -= S.IA * tau;
                wB += S.IB * tau;
            };

            solveMotorAxis(S.MotorImpulse.x, S.motorM_eff.x, S.e0);
            solveMotorAxis(S.MotorImpulse.y, S.motorM_eff.y, S.e1);
            solveMotorAxis(S.MotorImpulse.z, S.motorM_eff.z, S.e2);
        }

        if (S.coneLimitActive && S.coneM_eff > 0.f)
        {
            const float Jv = Dot(S.coneAxis, wB - wA);
            float dLambda = -S.coneM_eff * (Jv + S.coneBias);
            const float old = S.ConeLimitImpulse;
            S.ConeLimitImpulse = Max(old + dLambda, 0.f);
            dLambda = S.ConeLimitImpulse - old;
            const vec3 tau = S.coneAxis * dLambda;
            wA -= S.IA * tau;
            wB += S.IB * tau;
        }
    }
}

void BallJointBatch::Shutdown()
{
    foreachitem(S, FStates)
    {
        S.Joint->FPointImpulse = S.PointImpulse;
        S.Joint->FMotorImpulse = S.MotorImpulse;
        S.Joint->FConeLimitImpulse = S.ConeLimitImpulse;
    }
}

} // namespace Physics
} // namespace ECSEngine
