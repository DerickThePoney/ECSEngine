#include "stdafx.h"

#include "WeldJointBatch.h"

#include "Island.h"
#include "JointBatchHelpers.h"
#include "PhysicsEngine.h"
#include "RigidBody.h"

namespace ECSEngine
{
namespace Physics
{

IMPLEMENT_POOL_ALLOCATED(WeldJointState);

void WeldJointBatch::Initialize(Island* /*island*/)
{
    FStates.clear();
    FStates.reserve(FJoints.size());

    PhysicsEngine& Engine = PhysicsEngine::Instance();
    foreachitem(J, FJoints)
    {
        RigidBody* bodyA = Engine.GetRigidBody(J->FBodyA);
        RigidBody* bodyB = Engine.GetRigidBody(J->FBodyB);
        AssertRelease(bodyA && bodyB);

        WeldJointState& S = FStates.emplace_back();
        S.Joint = J;
        S.IndexA = bodyA->FIslandIndex;
        S.IndexB = bodyB->FIslandIndex;
        S.MA = bodyA->FInvMass;
        S.MB = bodyB->FInvMass;
        S.IA = bodyA->FInverseInertiaTensorWorld;
        S.IB = bodyB->FInverseInertiaTensorWorld;
        S.PointImpulse = J->FPointImpulse;
        S.AngularImpulse = J->FAngularImpulse;
    }
}

void WeldJointBatch::PreSolve(Island* island, float dt)
{
    PhysicsEngine& Engine = PhysicsEngine::Instance();
    foreachitem(S, FStates)
    {
        WeldJoint* J = S.Joint;
        RigidBody* bodyA = Engine.GetRigidBody(J->FBodyA);
        RigidBody* bodyB = Engine.GetRigidBody(J->FBodyB);
        AssertRelease(bodyA && bodyB);

        vec3& vA = island->FVelocities[S.IndexA].FLinearVelocity;
        vec3& wA = island->FVelocities[S.IndexA].FRotationVelocity;
        vec3& vB = island->FVelocities[S.IndexB].FLinearVelocity;
        vec3& wB = island->FVelocities[S.IndexB].FRotationVelocity;

        // Same three point axes as the ball joint: keep the two anchors on top of each other.
        S.xA = (bodyA->GetTransform() * vec4::MakeHomogeneousPositionVec4(J->FConfig.FCommon.FAnchorALocal)).xyz();
        S.xB = (bodyB->GetTransform() * vec4::MakeHomogeneousPositionVec4(J->FConfig.FCommon.FAnchorBLocal)).xyz();
        S.rA = S.xA - bodyA->FCenterOfMassWorld;
        S.rB = S.xB - bodyB->FCenterOfMassWorld;
        S.C = S.xB - S.xA;

        ComputeJointConstraintForI(S.M_eff.x, S.bias.x, vec3(1.f, 0.f, 0.f), S.C.x, S.MA, S.MB, S.IA, S.IB, S.rA, S.rB, dt);
        ComputeJointConstraintForI(S.M_eff.y, S.bias.y, vec3(0.f, 1.f, 0.f), S.C.y, S.MA, S.MB, S.IA, S.IB, S.rA, S.rB, dt);
        ComputeJointConstraintForI(S.M_eff.z, S.bias.z, vec3(0.f, 0.f, 1.f), S.C.z, S.MA, S.MB, S.IA, S.IB, S.rA, S.rB, dt);

        // Warm start. The stored impulse is applied before the iterations so the stack does not sag for a frame.
        {
            const vec3 P = S.PointImpulse;
            vA -= P * S.MA;
            wA -= S.IA * Cross(S.rA, P);
            vB += P * S.MB;
            wB += S.IB * Cross(S.rB, P);
        }

        // Angular axes are body A's frame. The error is measured in that frame, so each component lines up with one axis.
        const mat4 RA = mat4(bodyA->FOrientation);
        S.e0 = Normalize((RA * vec4::MakeHomogeneousDirectionVec4(vec3(1.f, 0.f, 0.f))).xyz());
        S.e1 = Normalize((RA * vec4::MakeHomogeneousDirectionVec4(vec3(0.f, 1.f, 0.f))).xyz());
        S.e2 = Normalize((RA * vec4::MakeHomogeneousDirectionVec4(vec3(0.f, 0.f, 1.f))).xyz());

        // qRel is B relative to A. qErr is how far that pose has drifted from the orientation captured at creation.
        // A positive component means B is ahead of the bind pose; the bias below drives wB - wA back toward zero.
        const quat qA = bodyA->FOrientation;
        const quat qB = bodyB->FOrientation;
        const quat qRel = JointQuatConjugate(qA) * qB;
        const quat qErr = JointQuatConjugate(J->FBindRelative) * qRel;
        S.angularC = JointQuatToRotVec(qErr);

        // Pure torque: no linear mass term, unlike the point constraint.
        auto angularMeff = [&](const vec3& e) -> float
        {
            const float K = Dot(e, S.IA * e) + Dot(e, S.IB * e);
            return (fabsf(K) < 1e-3f) ? 0.f : 1.f / K;
        };
        S.angularM_eff.x = angularMeff(S.e0);
        S.angularM_eff.y = angularMeff(S.e1);
        S.angularM_eff.z = angularMeff(S.e2);

        auto angularBias = [](float C, float parDT) -> float
        {
            return (fabsf(C) < JOINT_ANGULAR_SLOP) ? 0.f : (JOINT_BAUMGARTE / parDT) * C;
        };
        S.angularBias.x = angularBias(S.angularC.x, dt);
        S.angularBias.y = angularBias(S.angularC.y, dt);
        S.angularBias.z = angularBias(S.angularC.z, dt);

        // Warm start the orientation lock the same way as the anchors.
        {
            const vec3 tau = S.e0 * S.AngularImpulse.x + S.e1 * S.AngularImpulse.y + S.e2 * S.AngularImpulse.z;
            wA -= S.IA * tau;
            wB += S.IB * tau;
        }
    }
}

void WeldJointBatch::Solve(Island* island)
{
    foreachitem(S, FStates)
    {
        vec3& vA = island->FVelocities[S.IndexA].FLinearVelocity;
        vec3& wA = island->FVelocities[S.IndexA].FRotationVelocity;
        vec3& vB = island->FVelocities[S.IndexB].FLinearVelocity;
        vec3& wB = island->FVelocities[S.IndexB].FRotationVelocity;

        // Anchor velocity, including the tangential speed from each body's spin.
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

        // Uncapped, unlike the ball joint motor. A torque limit here lets the stack fold under thrust.
        auto solveAngularAxis = [&](float& impulseI, float M_effi, float biasi, const vec3& e)
        {
            if (M_effi <= 0.f)
                return;
            const float Jv = Dot(e, wB - wA);
            float dLambda = 0.f;
            ComputeJointImpulseForI(impulseI, dLambda, Jv, M_effi, biasi);
            const vec3 tau = e * dLambda;
            wA -= S.IA * tau;
            wB += S.IB * tau;
        };

        solveAngularAxis(S.AngularImpulse.x, S.angularM_eff.x, S.angularBias.x, S.e0);
        solveAngularAxis(S.AngularImpulse.y, S.angularM_eff.y, S.angularBias.y, S.e1);
        solveAngularAxis(S.AngularImpulse.z, S.angularM_eff.z, S.angularBias.z, S.e2);
    }
}

void WeldJointBatch::Shutdown()
{
    // Keep the accumulated impulses for next frame's warm start.
    foreachitem(S, FStates)
    {
        S.Joint->FPointImpulse = S.PointImpulse;
        S.Joint->FAngularImpulse = S.AngularImpulse;
    }
}

} // namespace Physics
} // namespace ECSEngine
