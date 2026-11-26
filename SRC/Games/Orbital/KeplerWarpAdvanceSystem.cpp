#include "stdafx.h"

#include "KeplerWarpAdvanceSystem.h"

#include "Common/TimeManager.h"
#include "ECSCore/ModuleAccessor.h"
#include "ECSCore/WorldIds.h"
#include "ECSGameplay_Common/RigidbodyModule.h"
#include "KeplerWarpModule.h"
#include "Physics/PhysicsAPI.h"
#include "RocketManagerModule.h"
#include "TimeWarpManager.h"

namespace ECSEngine
{

KeplerWarpAdvanceSystem::KeplerWarpAdvanceSystem()
{
    RegisterDependency<RocketManagerModule>(EEntityWorlds::STANDARD);
    RegisterDependency<RigidbodyModule>(EEntityWorlds::STANDARD);
    RegisterDependency<KeplerWarpModule>(EEntityWorlds::STANDARD);
}

void KeplerWarpAdvanceSystem::ExternalAdvanceBy(const double DT)
{
    LockControllers();
    AdvanceBy(DT);
    UnlockControllers();
}

void KeplerWarpAdvanceSystem::VirtualUpdate()
{
    ModuleSystem::VirtualUpdate();

    const u32 CurrentTimeWarpMult = TimeWarpManager::Instance().CurrentTimeWarpMultiplier();
    if (CurrentTimeWarpMult <= 1)
        return;

    AssertNotReached();
    const double DT = TimeManager::GameplayDeltaTime() * CurrentTimeWarpMult;
    AdvanceBy(DT);
}

void KeplerWarpAdvanceSystem::AdvanceBy(const double DT)
{
    ModuleAccessor<RocketManagerModule> rocketAccessor(EEntityWorlds::STANDARD);
    ModuleAccessor<KeplerWarpModule> keplerWarpAccessor(EEntityWorlds::STANDARD);
    ModuleAccessor<RigidbodyModule> rigidbodyAccessor(EEntityWorlds::STANDARD);

    foreachitem(rocket, rocketAccessor)
    {
        KeplerWarpModule* keplerModule = keplerWarpAccessor[rocket.UnitId()];
        AssertRelease(keplerModule != nullptr);

        KeplerPropagatorState& RocketState = keplerModule->KeplerState();
        if (!RocketState.FbActive)
            continue;

        const EntityId& PlanetId = rocket.CurrentGravityTarget();
        if (!PlanetId.Valid())
            continue;

        const RigidbodyModule* const planetBody = rigidbodyAccessor[PlanetId];
        AssertRelease(planetBody != nullptr);

        vec3 comPlanetPos, comPlanetVel;
        if (!Physics::GetBodyCenterOfMass(planetBody->BodyHandle(), comPlanetPos))
            continue;

        if (!Physics::GetBodyVelocity(planetBody->BodyHandle(), comPlanetVel))
            continue;

        const dvec3 comPlanetPosD(comPlanetPos.x, comPlanetPos.y, comPlanetPos.z);
        const dvec3 comPlanetVelD(comPlanetVel.x, comPlanetVel.y, comPlanetVel.z);

        dvec3 comRelativePos, comRelativeVel;
        if (!KeplerPropagator::Advance(RocketState, DT, comRelativePos, comRelativeVel))
        {
            TimeWarpManager::Instance().SetTimeWarpIndex(0);
            continue;
        }

        const dvec3 newComPosD = comRelativePos + comPlanetPosD;
        const dvec3 newComVelD = comRelativeVel + comPlanetVelD;

        keplerModule->SetPosition(newComPosD);
        keplerModule->SetVelocity(newComVelD);

        const vec3 newComPos(newComPosD.x, newComPosD.y, newComPosD.z);
        const vec3 newComVel(newComVelD.x, newComVelD.y, newComVelD.z);

        rocket.SetPositionAndVelocityForAllPartsUsingOffsets(rigidbodyAccessor, newComPos, newComVel, keplerModule->PartsOffsetConst());
    }
}

} // namespace ECSEngine
