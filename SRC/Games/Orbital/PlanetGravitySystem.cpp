#include "stdafx.h"

#include "PlanetGravitySystem.h"

#include "ECSCore/ModuleAccessor.h"
#include "ECSCore/WorldIds.h"
#include "ECSGameplay_Common/RigidbodyModule.h"
#include "OrbitalPathFollowerModule.h"
#include "Physics/PhysicsAPI.h"
#include "PlanetGravityModule.h"
#include "RocketManagerModule.h"

namespace ECSEngine
{
namespace PlanetGravityHelpers
{
vec3 ComputeGravityDueToPlanet(const vec3& pos, const vec3& planetPos, const float Mu)
{
    vec3 r = pos - planetPos;
    const float lr = Max(Length(r), 1.f);
    const float lr_3 = lr * lr * lr;
    return Invert(Mu * r / lr_3);
}

bool ApplyGravityOnActiveGroup(ModuleAccessor<RigidbodyModule>& rigidbodyAccessor,
      RocketManagerModule* rocket,
      const vec3& planetPos,
      const vec3& planetSpeedGravityTerm,
      const float Mu)
{
    vec3 vesselPosition;
    const bool hasVesselCenterOfMass = rocket->ComputeActiveCenterOfMass(rigidbodyAccessor, vesselPosition);
    AssertRelease(hasVesselCenterOfMass);
    if (!hasVesselCenterOfMass)
        return false;

    vec3 gravityAcceleration = ComputeGravityDueToPlanet(vesselPosition, planetPos, Mu);

    // Hackos
    rocket->SetLastTurnGravity(gravityAcceleration);

    // apply additional gravity term
    gravityAcceleration += planetSpeedGravityTerm;

    rocket->ApplyForceToAllActiveBodies(rigidbodyAccessor, gravityAcceleration, true);
    return true;
}

bool ApplyGravityOnDebris(ModuleAccessor<RigidbodyModule>& rigidbodyAccessor,
      RocketManagerModule* rocket,
      const vec3& planetPos,
      const vec3& planetSpeedGravityTerm,
      const float Mu)
{
    forrange(i, 0, rocket->DebrisCount())
    {
        vec3 debrisPos;
        const bool hasVesselCenterOfMass = rocket->ComputeCenterOfMassInDebrisIndex(rigidbodyAccessor, i, debrisPos);
        AssertRelease(hasVesselCenterOfMass);
        if (!hasVesselCenterOfMass)
            continue;

        vec3 gravityAcceleration = ComputeGravityDueToPlanet(debrisPos, planetPos, Mu) + planetSpeedGravityTerm;

        rocket->ApplyForceToAllBodiesInDebrisIndex(rigidbodyAccessor, i, gravityAcceleration, true);
    }
    return true;
}
} // namespace PlanetGravityHelpers

void PlanetGravitySystem::VirtualInit()
{
    ModuleSystem::VirtualInit();

    RegisterDependency<RigidbodyModule>(EEntityWorlds::STANDARD);
    RegisterDependency<PlanetGravityModule>(EEntityWorlds::STANDARD);
    RegisterDependency<RocketManagerModule>(EEntityWorlds::STANDARD);
    RegisterDependency<OrbitalPathFollowerModule>(EEntityWorlds::STANDARD);
}

void PlanetGravitySystem::VirtualUpdate()
{
    ModuleSystem::VirtualUpdate();

    ModuleAccessor<RocketManagerModule> rocketManagerAccessor(EEntityWorlds::STANDARD);
    ModuleAccessor<RigidbodyModule> rigidbodyAccessor(EEntityWorlds::STANDARD);
    ModuleAccessor<PlanetGravityModule> planetGravityAccessor(EEntityWorlds::STANDARD);
    ModuleAccessor<OrbitalPathFollowerModule> orbitFollowerAccessor(EEntityWorlds::STANDARD);

    foreachitem(rocket, rocketManagerAccessor)
    {
        const EntityId& CurrentTargetGravity = rocket.CurrentGravityTarget();
        if (!CurrentTargetGravity.Valid())
            continue;

        const PlanetGravityModule* planetGravity = planetGravityAccessor[CurrentTargetGravity];
        AssertRelease(planetGravity);

        const RigidbodyModule* const planetRB = rigidbodyAccessor[CurrentTargetGravity];
        AssertRelease(planetRB != nullptr);

        vec3 planetPosition;
        const bool hasPlanetCenter = Physics::GetBodyCenterOfMass(planetRB->BodyHandle(), planetPosition);
        AssertRelease(hasPlanetCenter);
        if (!hasPlanetCenter)
            continue;

        const OrbitalPathFollowerModule* const orbitFollowerModule = orbitFollowerAccessor[CurrentTargetGravity];
        vec3 planetSpeedGravityTerm;
        if (orbitFollowerModule != nullptr)
        {
            EntityId ParentPlanet = orbitFollowerModule->ParentObject();
            AssertRelease(ParentPlanet.Valid());

            const RigidbodyModule* const parentPlanetRB = rigidbodyAccessor[ParentPlanet];
            AssertRelease(parentPlanetRB != nullptr);

            vec3 parentPlanetPosition;
            const bool hasPlanetCenter = Physics::GetBodyCenterOfMass(parentPlanetRB->BodyHandle(), parentPlanetPosition);
            AssertRelease(hasPlanetCenter);

            vec3 Offset = planetPosition - parentPlanetPosition;
            planetSpeedGravityTerm -= float(orbitFollowerModule->MeanMotion() * orbitFollowerModule->MeanMotion()) * Offset;
        }

        PlanetGravityHelpers::ApplyGravityOnActiveGroup(rigidbodyAccessor, &rocket, planetPosition, planetSpeedGravityTerm, planetGravity->PlanetGravityConstant());
        PlanetGravityHelpers::ApplyGravityOnDebris(rigidbodyAccessor, &rocket, planetPosition, planetSpeedGravityTerm, planetGravity->PlanetGravityConstant());
    }
}
} // namespace ECSEngine
