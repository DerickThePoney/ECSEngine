#include "stdafx.h"

#include "ContactSolver.h"

#include "Contact.h"
#include "Island.h"
#include "RigidBody.h"

namespace ECSEngine
{
namespace Physics
{
static constexpr float BAUMGARTE = 0.2;
static constexpr float PENETRATION_SLOP = 0.05;

void ContactSolver::Initialise(Island* parIsland)
{
    FIsland = parIsland;
}

void ContactSolver::PreSolve(float parDeltaTime)
{
    foreachitem(CS, FIsland->FContactStates)
    {
        vec3 vA = FIsland->FVelocities[CS.IndexA].FLinearVelocity;
        vec3 wA = FIsland->FVelocities[CS.IndexA].FRotationVelocity;
        vec3 vB = FIsland->FVelocities[CS.IndexB].FLinearVelocity;
        vec3 wB = FIsland->FVelocities[CS.IndexB].FRotationVelocity;

        forrange(i, 0, CS.NumberOfContacts)
        {
            ContactPointState& CPS = CS.ContactPoints[i];

            // Precalculate JM^-1JT for contact and friction constraints
            vec3 raCn = Cross(CPS.CtoA, CS.Normal);
            vec3 rbCn = Cross(CPS.CtoB, CS.Normal);
            float nm = CS.MA + CS.MB;
            float tm[2];
            tm[0] = nm;
            tm[1] = nm;

            nm += Dot(raCn, CS.IA * raCn) + Dot(rbCn, CS.IB * rbCn);
            CPS.NormalMass = (nm != 0.f) ? 1.f / nm : 0.f;

            for (i32 i = 0; i < 2; ++i)
            {
                vec3 raCt = Cross(CS.TangentVectors[i], CPS.CtoA);
                vec3 rbCt = Cross(CS.TangentVectors[i], CPS.CtoB);
                tm[i] += Dot(raCt, CS.IA * raCt) + Dot(rbCt, CS.IB * rbCt);
                CPS.TangentMass[i] = (tm[i] != 0.f) ? 1.f / tm[i] : 0.f;
            }

            // Precalculate bias factor
            CPS.Bias = -BAUMGARTE * (1.0f / parDeltaTime) * Min(0.f, CPS.Penetration + PENETRATION_SLOP);

            // Warm start contact
            vec3 P = CS.Normal * CPS.NormalImpulse;

            /*if (m_enableFriction)
            {
                P += cs->tangentVectors[0] * c->tangentImpulse[0];
                P += cs->tangentVectors[1] * c->tangentImpulse[1];
            }*/

            vA -= P * CS.MA;
            wA -= CS.IA * Cross(CPS.CtoA, P);

            vB += P * CS.MB;
            wB += CS.IB * Cross(CPS.CtoB, P);

            // Add in restitution bias
            float dv = Dot(vB + Cross(wB, CPS.CtoB) - vA - Cross(wA, CPS.CtoA), CS.Normal);

            if (dv < -1.f)
                CPS.Bias += -(CS.Restitution) * dv;
        }

        // Write velocities back
        FIsland->FVelocities[CS.IndexA].FLinearVelocity = vA;
        FIsland->FVelocities[CS.IndexA].FRotationVelocity = wA;
        FIsland->FVelocities[CS.IndexB].FLinearVelocity = vB;
        FIsland->FVelocities[CS.IndexB].FRotationVelocity = wB;
    }
}

void ContactSolver::Solve()
{
    forrange(i, 0, FIsland->FContactStates.size())
    {
        // Get the contact state and its velocities
        ContactState& CS = FIsland->FContactStates[i];
        vec3 vA = FIsland->FVelocities[CS.IndexA].FLinearVelocity;
        vec3 wA = FIsland->FVelocities[CS.IndexA].FRotationVelocity;
        vec3 vB = FIsland->FVelocities[CS.IndexB].FLinearVelocity;
        vec3 wB = FIsland->FVelocities[CS.IndexB].FRotationVelocity;

        forrange(i, 0, CS.NumberOfContacts)
        {
            ContactPointState& CPS = CS.ContactPoints[i];

            // relative velocity at contact
            vec3 dv = vB + Cross(wB, CPS.CtoB) - vA - Cross(wA, CPS.CtoA);

            // TODO FRICTION

            // Normal Contact resolution
            {
                // Normal impulse
                float vn = Dot(dv, CS.Normal);

                // Factor in positional bias to calculate impulse scalar j
                float lambda = CPS.NormalMass * (-vn + CPS.Bias);

                // Clamp impulse
                float tempPN = CPS.NormalImpulse;
                CPS.NormalImpulse = Max(tempPN + lambda, 0.f);
                lambda = CPS.NormalImpulse - tempPN;

                // Apply impulse
                vec3 impulse = CS.Normal * lambda;
                vA -= impulse * CS.MA;
                wA -= CS.IA * Cross(CPS.CtoA, impulse);

                vB += impulse * CS.MB;
                wB += CS.IB * Cross(CPS.CtoB, impulse);
            }
        }

        // Write velocities back
        FIsland->FVelocities[CS.IndexA].FLinearVelocity = vA;
        FIsland->FVelocities[CS.IndexA].FRotationVelocity = wA;
        FIsland->FVelocities[CS.IndexB].FLinearVelocity = vB;
        FIsland->FVelocities[CS.IndexB].FRotationVelocity = wB;
    }
}

void ContactSolver::Shutdown()
{
    // TODO Write back the normal tangent impulses in the actual contacts
    forrange(i, 0, FIsland->FContacts.size())
    {
        ContactState& CS = FIsland->FContactStates[i];
        Contact* C = FIsland->FContacts[i];
        forrange(j, 0, CS.NumberOfContacts)
        {
            ContactPoint& CP = C->FManifold.FContactPoints[j];
            ContactPointState& CPS = CS.ContactPoints[j];
            CP.FNormalImpulse = CPS.NormalImpulse;
            CP.FTangentImpulse[0] = CPS.TangentImpulses[0];
            CP.FTangentImpulse[1] = CPS.TangentImpulses[1];
        }
    }
}

} // namespace Physics
} // namespace ECSEngine