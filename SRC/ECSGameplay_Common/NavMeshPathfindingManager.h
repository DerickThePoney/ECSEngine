#pragma once
#include "ECSCore/EntityId.h"

namespace ECSEngine
{
class Polygon2D;

struct PathfindingRequest
{
    EntityId UnitId;
    vec2 Start;
    vec2 End;
};

struct PathfindingResult
{
    EntityId UnitId;
    std::vector<vec2> Waypoints;
};

namespace Rendering
{
class DrawCommandBuffer;
class MaterialInstanceHandle;
} // namespace Rendering

namespace Pathfinding
{
void CreatePathfinder();
void InitialisePathfinder(const Polygon2D& parWorldExtent, const std::vector<Polygon2D>& parObstacles);
void ModifyWorldExtents(const Polygon2D& parWorldExtent);
void AddObstacle(const Polygon2D& parObstacle);
void DestroyPathfinder();

void PushRequest(PathfindingRequest&& parRequest);
PathfindingResult ComputeRequestSynchrone(PathfindingRequest&& parRequest);
void ComputeRequests();
void RetrieveResults(std::vector<PathfindingResult>& outResults);

void Debug(Rendering::DrawCommandBuffer& parBuffer, const Rendering::MaterialInstanceHandle& parMaterial);
} // namespace Pathfinding
} // namespace ECSEngine
