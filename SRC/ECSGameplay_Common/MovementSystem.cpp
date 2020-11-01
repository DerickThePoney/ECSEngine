#include "stdafx.h"

#include "MovementSystem.h"

#include "Common/RandomGenerator.h"
#include "Common/TimeManager.h"
#include "ECSCore/ModuleAccessor.h"
#include "MovementModule.h"
#include "PathfindingManager.h"
#include "PositionModule.h"

#include <glm/gtx/vec_swizzle.hpp>

namespace ECSEngine
{
namespace MovementHelpers
{
void GenerateNewPathfindRequest(MovementModule& parMovementModule, const PositionModule& parPositionModule)
{
    const float rx = RandomNumbers::NextFloat() * 200.0f - 100.0f;
    const float ry = RandomNumbers::NextFloat() * 200.0f - 100.0f;

    PathfindingRequest request;
    request.UnitId = parMovementModule.UnitId();
    request.Start = glm::xz(parPositionModule.GetPosition3D());
    request.End = glm::vec2(rx, ry);

    Pathfinding::PushRequest(std::move(request));
    parMovementModule.SetRequestIsPending(true);
}

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
    RegisterDepency<PositionModule>(Worlds::STANDARD);
    RegisterDepency<MovementModule>(Worlds::STANDARD);
}

MovementSystem::~MovementSystem()
{
}

void MovementSystem::VirtualUpdate()
{
    parent_type::VirtualUpdate();
    ModuleAccessor<MovementModule> movementModuleAccessor;
    ModuleAccessor<PositionModule> positionModuleAccessor;

    Pathfinding::ComputeRequests();
    std::vector<PathfindingResult> results;
    Pathfinding::RetrieveResults(results);

    const float dt = TimeManager::FrameDeltaTime();

    foreachitemconst(result, results)
    {
        MovementModule* movementModule = movementModuleAccessor[result.UnitId];
        if (movementModule != nullptr)
        {
            if (result.Waypoints.size() > 0)
                movementModule->SetNewPath(result.Waypoints);
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
                MovementHelpers::GenerateNewPathfindRequest(movementModule, *positionModule);
                movementModule.SetCurrentSpeed(glm::vec3(0.f));
            }
            else
            {
                MovementHelpers::FollowPath(movementModule, *positionModule, dt);
            }
        }
    }
}

void MovementSystem::VirtualInit()
{
    parent_type::VirtualInit();
}

} // namespace ECSEngine