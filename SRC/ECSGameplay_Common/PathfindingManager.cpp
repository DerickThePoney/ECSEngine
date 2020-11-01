#include "stdafx.h"

#include "PathfindingManager.h"

#include "Common/NavMesh.h"
#include "Common/NavMeshPath.h"
#include "Common/NavMeshPathSmoother.h"
#include "Common/NavMeshPathSolver.h"
#include "Common/NavMeshSolver.h"
#include "Common/Polygon.h"
#include "Common/Singleton.h"

namespace ECSEngine
{
class PathfindingManager : public Singleton<PathfindingManager>
{
public:
    void Initialise(const Polygon2D& parWorldExtent, const std::vector<Polygon2D>& parObstacles);
    void Cleanup();

    void PushRequest(PathfindingRequest&& parRequest);

    void ComputeRequests();

    void RetrieveResults(std::vector<PathfindingResult>& outResults);

private:
    void ProcessOneRequest(const PathfindingRequest& parRequest);

private:
    Navigation::NavMesh FNavMesh;

    std::queue<PathfindingRequest> FRequests;
    std::vector<PathfindingResult> FResults;
};

void PathfindingManager::Initialise(const Polygon2D& parWorldExtent, const std::vector<Polygon2D>& parObstacles)
{
    if (FNavMesh.VerticesCount() > 0)
        FNavMesh.Cleanup();

    Navigation::NavMeshSolver solver;
    solver.CreateNavMesh(parWorldExtent, parObstacles, FNavMesh);
}

void PathfindingManager::Cleanup()
{
    FNavMesh.Cleanup();
}

void PathfindingManager::PushRequest(PathfindingRequest&& parRequest)
{
    FRequests.push(parRequest);
}

void PathfindingManager::ComputeRequests()
{
    while (!FRequests.empty())
    {
        PathfindingRequest request = FRequests.front();
        FRequests.pop();
        ProcessOneRequest(request);
    }
}

void PathfindingManager::RetrieveResults(std::vector<PathfindingResult>& outResults)
{
    outResults.swap(FResults);
    FResults.clear();
}

void PathfindingManager::ProcessOneRequest(const PathfindingRequest& parRequest)
{
    Navigation::NavMeshPath path(parRequest.Start, parRequest.End);
    Navigation::NavMeshPathSolver pathSolver;
    pathSolver.SolvePath(FNavMesh, path);

    PathfindingResult result;
    result.UnitId = parRequest.UnitId;

    if (path.isValid())
    {
        Navigation::NavMeshPathSmoother smoother;
        smoother.SmoothPath(FNavMesh, path);

        result.Waypoints.reserve(path.size());
        result.Waypoints.push_back(path.Start());
        foreachitemconst(vertex, path) { result.Waypoints.push_back(vertex->Position); }
        result.Waypoints.push_back(path.End());
    }
    FResults.push_back(std::move(result));
}

namespace Pathfinding
{

void CreatePathfinder()
{
    PathfindingManager::CreateIFP();
}

void InitialisePathfinder(const Polygon2D& parWorldExtent, const std::vector<Polygon2D>& parObstacles)
{
    PathfindingManager::Instance().Initialise(parWorldExtent, parObstacles);
}

void DestroyPathfinder()
{
    PathfindingManager::Instance().Cleanup();
    PathfindingManager::Destroy();
}

void PushRequest(PathfindingRequest&& parRequest)
{
    PathfindingManager::Instance().PushRequest(std::move(parRequest));
}

void ComputeRequests()
{
    PathfindingManager::Instance().ComputeRequests();
}

void RetrieveResults(std::vector<PathfindingResult>& outResults)
{
    PathfindingManager::Instance().RetrieveResults(outResults);
}

} // namespace Pathfinding
} // namespace ECSEngine
