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
        vec3 vesselPosition;
        const bool hasVesselCenterOfMass = rocket.ComputeCenterOfMass(rigidbodyAccessor, vesselPosition);
        AssertRelease(hasVesselCenterOfMass);
        if (!hasVesselCenterOfMass)
            continue;

        vec3 gravityAcceleration{ 0.f };

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

        vec3 r = vesselPosition - planetPosition;
        const float lr = Max(Length(r), 1.f);
        const float lr_3 = lr * lr * lr;
        gravityAcceleration -= planetGravity->PlanetGravityConstant() * r / lr_3;

        // Hackos
        rocket.SetLastTurnGravity(gravityAcceleration);

        const OrbitalPathFollowerModule* const orbitFollowerModule = orbitFollowerAccessor[CurrentTargetGravity];
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
            gravityAcceleration -= float(orbitFollowerModule->MeanMotion() * orbitFollowerModule->MeanMotion()) * Offset;
        }

        rocket.ApplyForceToAllBodies(rigidbodyAccessor, gravityAcceleration, true);
    }
}
} // namespace ECSEngine
