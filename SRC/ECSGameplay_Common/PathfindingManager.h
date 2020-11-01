#pragma once
#include "ECSCore/EntityId.h"

namespace ECSEngine
{
class Polygon2D;

struct PathfindingRequest
{
    EntityId UnitId;
    glm::vec2 Start;
    glm::vec2 End;
};

struct PathfindingResult
{
    EntityId UnitId;
    std::vector<glm::vec2> Waypoints;
};

namespace Pathfinding
{
void CreatePathfinder();
void InitialisePathfinder(const Polygon2D& parWorldExtent, const std::vector<Polygon2D>& parObstacles);
void DestroyPathfinder();

void PushRequest(PathfindingRequest&& parRequest);
void ComputeRequests();
void RetrieveResults(std::vector<PathfindingResult>& outResults);
} // namespace Pathfinding
} // namespace ECSEngine