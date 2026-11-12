#include "stdafx.h"

#include "PlanetOrbitalPathFollowingSystem.h"

#include "Common/TimeManager.h"
#include "ECSCore/ModuleAccessor.h"
#include "ECSCore/WorldIds.h"
#include "ECSGameplay_Common/RigidbodyModule.h"
#include "OrbitalPathFollowerModule.h"
#include "Physics/PhysicsAPI.h"
#include "PlanetGravityModule.h"

namespace ECSEngine
{
PlanetOrbitalPathFollowingSystem::PlanetOrbitalPathFollowingSystem()
{
    RegisterDependency<OrbitalPathFollowerModule>(EEntityWorlds::STANDARD);
    RegisterDependency<PlanetGravityModule>(EEntityWorlds::STANDARD);
    RegisterDependency<RigidbodyModule>(EEntityWorlds::STANDARD);
}

void PlanetOrbitalPathFollowingSystem::VirtualUpdate()
{
    ModuleSystem::VirtualUpdate();

    ModuleAccessor<OrbitalPathFollowerModule> pathFollowerAccessor(EEntityWorlds::STANDARD);
    ModuleAccessor<PlanetGravityModule> planetGravityAccessor(EEntityWorlds::STANDARD);
    ModuleAccessor<RigidbodyModule> rigidbodyAccessor(EEntityWorlds::STANDARD);

    const float DT = TimeManager::GameplayDeltaTime();

    foreachitem(pathFollower, pathFollowerAccessor)
    {
        PlanetGravityModule* planetGravity = planetGravityAccessor[pathFollower.UnitId()];
        if (planetGravity == nullptr)
        {
            continue;
        }

        double CurrentTime = pathFollower.CurrentTime();
        if (!pathFollower.Initialized())
        {
            PlanetGravityModule* parentPlanetGravity = planetGravityAccessor[pathFollower.ParentObject()];
            AssertRelease(parentPlanetGravity != nullptr);

            const float mu_parent = parentPlanetGravity->PlanetGravityConstant();
            const double n = sqrt((double)mu_parent / pow(((double)pathFollower.OrbitRadius()), 3));
            pathFollower.SetMeanMotion(n);
            pathFollower.SetInitialized();
        }
        else
        {
            CurrentTime += DT;
            pathFollower.SetCurrentTime(CurrentTime);
        }

        const double meanMotionDelta = pathFollower.MeanMotion() * CurrentTime;
        const double cosPart = Cos(meanMotionDelta + pathFollower.OrbitPhase());
        const double sinPart = Sin(meanMotionDelta + pathFollower.OrbitPhase());

        RigidbodyModule* parentBody = rigidbodyAccessor[pathFollower.ParentObject()];
        AssertRelease(parentBody != nullptr);

        vec3 ParentPosition;
        if (!Physics::GetBodyPosition(parentBody->BodyHandle(), ParentPosition))
        {
            AssertNotReached();
            continue;
        }

        vec3 NewPosition = ParentPosition + pathFollower.OrbitRadius() * vec3(cosPart, 0.f, sinPart);
        vec3 NewVelocity = float(pathFollower.MeanMotion() * pathFollower.OrbitRadius()) * vec3(-sinPart, 0.f, cosPart);

        RigidbodyModule* body = rigidbodyAccessor[pathFollower.UnitId()];
        AssertRelease(body != nullptr);

        Physics::SetBodyPosition(body->BodyHandle(), NewPosition);
        Physics::SetBodyVelocity(body->BodyHandle(), NewVelocity);
    }
}
} // namespace ECSEngine