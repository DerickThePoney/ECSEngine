#include "stdafx.h"

#include "PlanetOrbitalPathFollowingSystem.h"

#include "Common/TimeManager.h"
#include "ECSCore/ModuleAccessor.h"
#include "ECSCore/WorldIds.h"
#include "ECSGameplay_Common/RigidbodyModule.h"
#include "OrbitalPathFollowerModule.h"
#include "Physics/PhysicsAPI.h"
#include "PlanetGravityModule.h"
#include "RenderingCore/DrawCommands.h"
#include "TimeWarpManager.h"

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

    const double DT = (double)TimeManager::GameplayDeltaTime();

    AdvanceBy(DT);
}

void PlanetOrbitalPathFollowingSystem::AdvanceBy(const double DT)
{
    ModuleAccessor<OrbitalPathFollowerModule> pathFollowerAccessor(EEntityWorlds::STANDARD);
    ModuleAccessor<PlanetGravityModule> planetGravityAccessor(EEntityWorlds::STANDARD);
    ModuleAccessor<RigidbodyModule> rigidbodyAccessor(EEntityWorlds::STANDARD);

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

void PlanetOrbitalPathFollowingSystem::ExternalAdvanceBy(const double DT)
{
    LockControllers();
    AdvanceBy(DT);
    UnlockControllers();
}

void PlanetOrbitalPathFollowingSystem::DrawDebug(Rendering::DrawCommandBuffer* buffer, const Rendering::MaterialInstanceHandle& material)
{
    FOrbitRings.clear();

    LockControllers();

    ModuleAccessor<OrbitalPathFollowerModule> pathFollowerAccessor(EEntityWorlds::STANDARD);
    ModuleAccessor<RigidbodyModule> rigidbodyAccessor(EEntityWorlds::STANDARD);

    foreachitem(pathFollower, pathFollowerAccessor)
    {
        RigidbodyModule* parentBody = rigidbodyAccessor[pathFollower.ParentObject()];
        if (parentBody == nullptr)
            continue;

        vec3 parentPosition;
        if (!Physics::GetBodyPosition(parentBody->BodyHandle(), parentPosition))
            continue;

        constexpr u32 sampleCount = 64;
        std::vector<vec3> points(sampleCount);
        const float radius = pathFollower.OrbitRadius();
        for (u32 i = 0; i < sampleCount; ++i)
        {
            const float theta = (float)i / (float)sampleCount * 2.f * Pi();
            points[i] = parentPosition + radius * vec3(Cos(theta), 0.f, Sin(theta));
        }
        FOrbitRings.push_back(std::move(points));
    }

    UnlockControllers();

    for (const std::vector<vec3>& points : FOrbitRings)
        buffer->DrawLines(material, points.data(), (u32)points.size(), 0xFF66CCFFu, true);
}

} // namespace ECSEngine