#include "stdafx.h"

#include "KeplerWarpCaptureSystem.h"

#include "ECSCore/ModuleAccessor.h"
#include "ECSCore/WorldIds.h"
#include "ECSGameplay_Common/RigidbodyModule.h"
#include "KeplerWarpModule.h"
#include "Physics/PhysicsAPI.h"
#include "PlanetGravityModule.h"
#include "RocketManagerModule.h"
#include "TimeWarpManager.h"

namespace ECSEngine
{

KeplerWarpCaptureSystem::KeplerWarpCaptureSystem()
{
    RegisterDependency<RocketManagerModule>(EEntityWorlds::STANDARD);
    RegisterDependency<RigidbodyModule>(EEntityWorlds::STANDARD);
    RegisterDependency<KeplerWarpModule>(EEntityWorlds::STANDARD);
    RegisterDependency<PlanetGravityModule>(EEntityWorlds::STANDARD);
}

void KeplerWarpCaptureSystem::VirtualUpdate()
{
    ModuleSystem::VirtualUpdate();

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

    foreachitem(rocket, rocketAccessor)
    {
        KeplerWarpModule* keplerModule = keplerWarpAccessor[rocket.UnitId()];
        AssertRelease(keplerModule != nullptr);

        KeplerPropagatorState& RocketState = keplerModule->KeplerState();

        if (bJustExited)
        {
            RocketState.FbActive = false;
            keplerModule->SetCapturedParent(EntityId());
            continue;
        }

        const EntityId& PlanetId = rocket.CurrentGravityTarget();
        if (!PlanetId.Valid())
            continue;

        const bool bParentChanged = RocketState.FbActive && keplerModule->CapturedParent() != PlanetId;
        if (!bJustEntered && RocketState.FbActive && !bParentChanged)
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

        const dvec3 comPlanetPosD(comPlanetPos.x, comPlanetPos.y, comPlanetPos.z);
        const dvec3 comPlanetVelD(comPlanetVel.x, comPlanetVel.y, comPlanetVel.z);

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

        bSuccess = Physics::SetBodyRotationVelocity(bodyBody->BodyHandle(), vec3{ 0.f });
        AssertRelease(bSuccess);
        bSuccess = Physics::SetBodyRotationVelocity(nozzleBody->BodyHandle(), vec3{ 0.f });
        AssertRelease(bSuccess);

        const dvec3 comPosD(comPos.x, comPos.y, comPos.z);
        const dvec3 comVelD(comVel.x, comVel.y, comVel.z);
        keplerModule->SetPosition(comPosD);
        keplerModule->SetVelocity(comVelD);
        keplerModule->SetOffsetBody(bodyPos - comPos);
        keplerModule->SetOffsetNozzle(nozzlePos - comPos);

        const bool bCaptured = KeplerPropagator::TryCapture(RocketState, comPlanetPosD, comPlanetVelD, comPosD, comVelD, planetGravity->PlanetGravityConstant());

        if (!bCaptured)
        {
            keplerModule->SetCapturedParent(EntityId());
            keplerModule->SetSkipAdvanceOnce(false);
            TimeWarpManager::Instance().SetTimeWarpIndex(0);
        }
        else
        {
            keplerModule->SetCapturedParent(PlanetId);
            // Same-tick Advance after capture was jumping; hold pose until next tick
            keplerModule->SetSkipAdvanceOnce(true);
        }
    }
}

} // namespace ECSEngine
