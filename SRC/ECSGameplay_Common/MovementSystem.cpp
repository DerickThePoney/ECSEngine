#include "stdafx.h"

#include "MovementSystem.h"

#include "Common/RandomGenerator.h"
#include "Common/RenderingHandles.h"
#include "Common/TimeManager.h"
#include "ECSCore/AdjustableDebugParameters.h"
#include "ECSCore/ModuleAccessor.h"
#include "MovementModule.h"
#include "NavMeshPathfindingManager.h"
#include "PositionModule.h"
#include "RenderingCore/DrawCommands.h"

#include <glm/gtx/vec_swizzle.hpp>
#include "WorldIds.h"

namespace ECSEngine
{
namespace MovementHelpers
{
void FollowPath(MovementModule& parMovementModule, PositionModule& parPositionModule, const float parDT)
{
    glm::vec3 currentPosition = parPositionModule.GetPosition3D();

    const std::vector<glm::vec2>& path = parMovementModule.Path();
    u32 followedWaypoint = parMovementModule.CurrentFollowedWayPoint();

    if (followedWaypoint >= path.size())
    {
        parMovementModule.SetCurrentFollowedWaypoint(-1);
        return;
    }

    glm::vec3 currentWaypoint = glm::vec3(path[followedWaypoint].x, 0.f, path[followedWaypoint].y);

    while (glm::length2(currentPosition - currentWaypoint) < 0.1f * 0.1f)
    {
        followedWaypoint += 1;
        if (followedWaypoint >= path.size())
        {
            parMovementModule.SetCurrentFollowedWaypoint(-1);
            return;
        }
        currentWaypoint = glm::vec3(path[followedWaypoint].x, 0.f, path[followedWaypoint].y);
    }

    parMovementModule.SetCurrentFollowedWaypoint(followedWaypoint);

    const glm::vec3 direction = glm::normalize(currentWaypoint - currentPosition);
    const MovementModuleTemplate* movementModuleTemplate = parMovementModule.Template<MovementModuleTemplate>();
    AssertRelease(movementModuleTemplate != nullptr);
    const glm::vec3 speed = movementModuleTemplate->MaxSpeed() * direction;
    currentPosition += speed * parDT;

    parMovementModule.SetCurrentSpeed(speed);
    parPositionModule.SetPosition3D(currentPosition);
}
} // namespace MovementHelpers

MovementSystem::MovementSystem()
    : parent_type()
{
    RegisterDepency<PositionModule>(EEntityWorlds::PEONS);
    RegisterDepency<MovementModule>(EEntityWorlds::PEONS);
}

MovementSystem::~MovementSystem()
{
}

void MovementSystem::VirtualInit()
{
    parent_type::VirtualInit();
}

void MovementSystem::VirtualUpdate()
{
    parent_type::VirtualUpdate();
    ModuleAccessor<MovementModule> movementModuleAccessor(EEntityWorlds::PEONS);
    ModuleAccessor<PositionModule> positionModuleAccessor(EEntityWorlds::PEONS);

    Pathfinding::ComputeRequests();
    std::vector<PathfindingResult> results;
    Pathfinding::RetrieveResults(results);

    const float dt = TimeManager::GameplayDeltaTime();

    foreachitemconst(result, results)
    {
        MovementModule* movementModule = movementModuleAccessor[result.UnitId];
        if (movementModule != nullptr)
        {
            if (result.Waypoints.size() > 0)
                movementModule->SetNewPath(result.Waypoints);
            else
                movementModule->ClearPath();
            movementModule->SetRequestIsPending(false);
        }
    }

    foreachitem(movementModule, movementModuleAccessor)
    {
        PositionModule* positionModule = positionModuleAccessor[movementModule.UnitId()];
        AssertRelease(positionModule != nullptr);
        if (!movementModule.RequestIsPending())
        {
            const std::vector<glm::vec2>& path = movementModule.Path();
            u32 followedWaypoint = movementModule.CurrentFollowedWayPoint();
            if (path.size() == 0 || followedWaypoint == -1)
            {
                movementModule.SetCurrentSpeed(glm::vec3(0.f));
            }
            else
            {
                MovementHelpers::FollowPath(movementModule, *positionModule, dt);
            }
        }
    }
}

void MovementSystem::VisualDebug(Rendering::DrawCommandBuffer& parBuffer, const Rendering::MaterialInstanceHandle& parMaterial)
{
    ADJUSTABLE_DEBUG_PARAMETER_BOOLEAN(showUnitPath, false, "Show Unit Path", "Movement/Path");
    if (!showUnitPath)
        return;

    LockControllers();

    ModuleAccessor<MovementModule> movementModuleAccessor(EEntityWorlds::PEONS);
    ModuleAccessor<PositionModule> positionModuleAccessor(EEntityWorlds::PEONS);

    foreachitem(movementModule, movementModuleAccessor)
    {
        const PositionModule* positionModule = positionModuleAccessor[movementModule.UnitId()];
        AssertRelease(positionModule != nullptr);
        if (!movementModule.RequestIsPending())
        {
            u32 size = 0;
            const glm::vec3* pathForDebug = movementModule.GetPathForDebug(positionModule->GetPosition3D(), size);

            if (size == 0)
                continue;

            parBuffer.DrawLines(parMaterial, pathForDebug, size, 0xFF00FFFF, false);

            std::vector<glm::vec2> passedWaypoints;
            const std::vector<glm::vec2>& path = movementModule.Path();
            const u32 followedWaypoint = movementModule.CurrentFollowedWayPoint();

            if (followedWaypoint != -1)
            {
                forrange(i, 0, followedWaypoint) passedWaypoints.push_back(path[i]);
                passedWaypoints.push_back(glm::xz(positionModule->GetPosition3D()));

                parBuffer.DrawLines(parMaterial, passedWaypoints, (u32)passedWaypoints.size(), 0.0f, 0xFFFF00FF, false);
            }

            parBuffer.DrawLines(parMaterial, { path.front(), path.back() }, 2, 0.f, 0xFF0000FF, false);
        }
    }

    UnlockControllers();
}

} // namespace ECSEngine
