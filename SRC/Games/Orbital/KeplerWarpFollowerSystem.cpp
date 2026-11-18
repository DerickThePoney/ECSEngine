#include "stdafx.h"

#include "KeplerWarpFollowerSystem.h"

#include "Common/TimeManager.h"
#include "ECSCore/ModuleAccessor.h"
#include "ECSCore/WorldIds.h"
#include "ECSGameplay_Common/PositionModule.h"
#include "ECSGameplay_Common/RigidbodyModule.h"
#include "KeplerWarpModule.h"
#include "Physics/PhysicsAPI.h"
#include "PlanetGravityModule.h"
#include "RocketManagerModule.h"
#include "TimeWarpManager.h"

namespace ECSEngine
{

KeplerWarpFollowerSystem::KeplerWarpFollowerSystem()
{
    RegisterDependency<RocketManagerModule>(EEntityWorlds::STANDARD);
    RegisterDependency<RigidbodyModule>(EEntityWorlds::STANDARD);
    RegisterDependency<KeplerWarpModule>(EEntityWorlds::STANDARD);
    RegisterDependency<PlanetGravityModule>(EEntityWorlds::STANDARD);
    RegisterDependency<PositionModule>(EEntityWorlds::STANDARD);
}

void KeplerWarpFollowerSystem::VirtualUpdate()
{
    ModuleSystem::VirtualUpdate();

    // timewarp edge detection
    const u32 CurrentTimeWarpMult = TimeWarpManager::Instance().CurrentTimeWarpMultiplier();
    bool bJustEntered = false;
    bool bJustExited = false;

    if (CurrentTimeWarpMult == 1 && PreviousMultiplier > 1)
    {
        bJustExited = true;
    }
    else if (PreviousMultiplier == 1 && CurrentTimeWarpMult > 1)
    {
        bJustEntered = true;
    }
    PreviousMultiplier = CurrentTimeWarpMult;

    if (CurrentTimeWarpMult == 1 && !bJustExited)
        return;

    ModuleAccessor<RocketManagerModule> rocketAccessor(EEntityWorlds::STANDARD);
    ModuleAccessor<KeplerWarpModule> keplerWarpAccessor(EEntityWorlds::STANDARD);
    ModuleAccessor<RigidbodyModule> rigidbodyAccessor(EEntityWorlds::STANDARD);
    ModuleAccessor<PlanetGravityModule> planetGravityAccessor(EEntityWorlds::STANDARD);
    ModuleAccessor<PositionModule> positionAccessor(EEntityWorlds::STANDARD);

    const double DT = TimeManager::GameplayDeltaTime() * CurrentTimeWarpMult;
    foreachitem(rocket, rocketAccessor)
    {
        KeplerWarpModule* keplerModule = keplerWarpAccessor[rocket.UnitId()];
        AssertRelease(keplerModule != nullptr);

        if (bJustExited)
        {
            keplerModule->KeplerState().FbActive = false;
            continue;
        }

        // parent Position
        const EntityId& PlanetId = rocket.CurrentGravityTarget();
        if (!PlanetId.Valid())
            continue;

        const PlanetGravityModule* const planetGravity = planetGravityAccessor[PlanetId];
        AssertRelease(planetGravity != nullptr);

        const RigidbodyModule* const planetBody = rigidbodyAccessor[PlanetId];
        AssertRelease(planetBody != nullptr);

        vec3 comPlanetPos, comPlanetVel;
        if (!Physics::GetBodyCenterOfMass(planetBody->BodyHandle(), comPlanetPos))
            continue;

        if (!Physics::GetBodyVelocity(planetBody->BodyHandle(), comPlanetVel))
            continue;

        dvec3 comPlanetPosD, comPlanetVelD;
        comPlanetPosD = dvec3(comPlanetPos.x, comPlanetPos.y, comPlanetPos.z);
        comPlanetVelD = dvec3(comPlanetVel.x, comPlanetVel.y, comPlanetVel.z);

        // if kepler is inactive: capture position, velocity, and state
        KeplerPropagatorState& RocketState = keplerModule->KeplerState();
        if (bJustEntered || !RocketState.FbActive)
        {
            // rocket position
            vec3 comPos, comVel;
            if (!rocket.ComputeCenterOfMass(rigidbodyAccessor, comPos, &comVel))
                continue;

            const RigidbodyModule* const bodyBody = rigidbodyAccessor[rocket.BodyId()];
            AssertRelease(bodyBody != nullptr);

            const RigidbodyModule* const nozzleBody = rigidbodyAccessor[rocket.NozzleId()];
            AssertRelease(nozzleBody != nullptr);

            vec3 bodyPos, nozzlePos;
            bool bSuccess = Physics::GetBodyPosition(bodyBody->BodyHandle(), bodyPos);
            AssertRelease(bSuccess);
            bSuccess = Physics::GetBodyPosition(nozzleBody->BodyHandle(), nozzlePos);
            AssertRelease(bSuccess);

            // freeze orientation
            bSuccess = Physics::SetBodyRotationVelocity(bodyBody->BodyHandle(), vec3{ 0.f });
            AssertRelease(bSuccess);
            bSuccess = Physics::SetBodyRotationVelocity(nozzleBody->BodyHandle(), vec3{ 0.f });
            AssertRelease(bSuccess);

            keplerModule->SetPosition(dvec3(comPos.x, comPos.y, comPos.z));
            keplerModule->SetVelocity(dvec3(comVel.x, comVel.y, comVel.z));

            keplerModule->SetOffsetBody(bodyPos - comPos);
            keplerModule->SetOffsetNozzle(nozzlePos - comPos);

            if (!KeplerPropagator::TryCapture(
                      RocketState, comPlanetPosD, comPlanetVelD, keplerModule->Position(), keplerModule->Velocity(), planetGravity->PlanetGravityConstant()))
            {
                TimeWarpManager::Instance().SetTimeWarpIndex(0);
            }
        }

        // if kepler is active ==> use current position and vel and advance
        if (RocketState.FbActive)
        {
            dvec3 comRelativePos, comRelativeVel;
            if (KeplerPropagator::Advance(RocketState, DT, comRelativePos, comRelativeVel))
            {

                // plug this back into the system
                dvec3 newComPosD, newComVelD;
                newComPosD = comRelativePos + comPlanetPosD;
                newComVelD = comRelativeVel + comPlanetVelD;
                keplerModule->SetPosition(newComPosD);
                keplerModule->SetVelocity(newComVelD);

                // Set body and nozzle pos back
                vec3 newComPos, newComVel, bodyPos, nozzlePos;
                newComPos = vec3(newComPosD.x, newComPosD.y, newComPosD.z);
                newComVel = vec3(newComVelD.x, newComVelD.y, newComVelD.z);
                bodyPos = newComPos + keplerModule->OffsetBody();
                nozzlePos = newComPos + keplerModule->OffsetNozzle();

                // push the bodies new pos
                const RigidbodyModule* const bodyBody = rigidbodyAccessor[rocket.BodyId()];
                AssertRelease(bodyBody != nullptr);

                const RigidbodyModule* const nozzleBody = rigidbodyAccessor[rocket.NozzleId()];
                AssertRelease(nozzleBody != nullptr);

                PositionModule* const bodyPosMod = positionAccessor[rocket.BodyId()];
                AssertRelease(bodyPosMod != nullptr);

                PositionModule* const nozzlePosMod = positionAccessor[rocket.NozzleId()];
                AssertRelease(nozzlePosMod != nullptr);

                Physics::SetBodyPosition(bodyBody->BodyHandle(), bodyPos);
                Physics::SetBodyVelocity(bodyBody->BodyHandle(), newComVel);
                Physics::SetBodyPosition(nozzleBody->BodyHandle(), nozzlePos);
                Physics::SetBodyVelocity(nozzleBody->BodyHandle(), newComVel);
                bodyPosMod->SetPosition3D(bodyPos);
                nozzlePosMod->SetPosition3D(nozzlePos);
            }
            else
            {
                TimeWarpManager::Instance().SetTimeWarpIndex(0);
            }
        }
    }
}

} // namespace ECSEngine
