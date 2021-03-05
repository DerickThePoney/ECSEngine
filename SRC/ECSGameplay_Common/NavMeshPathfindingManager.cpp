#include "stdafx.h"

#include "NavMeshPathfindingManager.h"

#include "Common/NavMesh.h"
#include "Common/NavMeshPath.h"
#include "Common/NavMeshPathSmoother.h"
#include "Common/NavMeshPathSolver.h"
#include "Common/NavMeshSolver.h"
#include "Common/Polygon.h"
#include "Common/Singleton.h"
#include "ECSCore/AdjustableDebugParameters.h"
#include "RenderingCore/DrawCommands.h"

namespace ECSEngine
{

class NavMeshPathfindingManager : public Singleton<NavMeshPathfindingManager>
{
public:
    void Initialise(const Polygon2D& parWorldExtent, const std::vector<Polygon2D>& parObstacles);
    void ModifyWorldExtents(const Polygon2D& parWorldExtent);
    void AddObstacle(const Polygon2D& parObstacle);
    void Cleanup();

    void PushRequest(PathfindingRequest&& parRequest);

    PathfindingResult ComputeRequestsSynchrone(PathfindingRequest&& parRequest);

    void ComputeRequests();

    void RetrieveResults(std::vector<PathfindingResult>& outResults);

    void Debug(Rendering::DrawCommandBuffer& parBuffer, const Rendering::MaterialInstanceHandle& parMaterial);

private:
    void ProcessOneRequest(const PathfindingRequest& parRequest);

private:
    Navigation::NavMesh FNavMesh;

    std::queue<PathfindingRequest> FRequests;
    std::vector<PathfindingResult> FResults;
};

void NavMeshPathfindingManager::Initialise(const Polygon2D& parWorldExtent, const std::vector<Polygon2D>& parObstacles)
{
    if (FNavMesh.VerticesCount() > 0)
        FNavMesh.Cleanup();

    Navigation::NavMeshSolver solver;
    solver.CreateNavMesh(parWorldExtent, parObstacles, FNavMesh);
}

void NavMeshPathfindingManager::ModifyWorldExtents(const Polygon2D& parWorldExtent)
{
    MemoryView<const Polygon2D> navMeshHolesView = FNavMesh.Holes();
    std::vector<Polygon2D> navMeshHolesCopy;
    navMeshHolesCopy.reserve(navMeshHolesView.size());
    foreachitemconst(hole, navMeshHolesView) { navMeshHolesCopy.push_back(hole); }

    Initialise(parWorldExtent, navMeshHolesCopy);
}

void NavMeshPathfindingManager::AddObstacle(const Polygon2D& parObstacle)
{
    MemoryView<const Polygon2D> navMeshHolesView = FNavMesh.Holes();
    std::vector<Polygon2D> navMeshHolesCopy;
    navMeshHolesCopy.reserve(navMeshHolesView.size() + 1);
    foreachitemconst(hole, navMeshHolesView) { navMeshHolesCopy.push_back(hole); }

    navMeshHolesCopy.push_back(parObstacle);

    Initialise(FNavMesh.MainPolygon(), navMeshHolesCopy);
}

void NavMeshPathfindingManager::Cleanup()
{
    FNavMesh.Cleanup();
}

void NavMeshPathfindingManager::PushRequest(PathfindingRequest&& parRequest)
{
    FRequests.push(parRequest);
}

PathfindingResult NavMeshPathfindingManager::ComputeRequestsSynchrone(PathfindingRequest&& parRequest)
{
    const PathfindingRequest request = parRequest;
    ProcessOneRequest(parRequest);
    PathfindingResult res = FResults.back();
    FResults.pop_back();
    return res;
}

void NavMeshPathfindingManager::ComputeRequests()
{
    while (!FRequests.empty())
    {
        PathfindingRequest request = FRequests.front();
        FRequests.pop();
        ProcessOneRequest(request);
    }
}

void NavMeshPathfindingManager::RetrieveResults(std::vector<PathfindingResult>& outResults)
{
    outResults.swap(FResults);
    FResults.clear();
}

void NavMeshPathfindingManager::Debug(Rendering::DrawCommandBuffer& parBuffer, const Rendering::MaterialInstanceHandle& parMaterial)
{
    ADJUSTABLE_DEBUG_PARAMETER_BOOLEAN(showPathfindingDebug, false, "Show navmesh", "Pathfinding/NavMesh");
    if (!showPathfindingDebug || FNavMesh.FacesCount() == 0)
        return;

    const glm::vec2* mainPolygon = FNavMesh.MainPolygon().data().data();
    parBuffer.DrawLines(parMaterial, mainPolygon, (u32)FNavMesh.MainPolygon().size(), 0.0f, 0xFF00FF00, true);

    MemoryView<const Polygon2D> memView = FNavMesh.Holes();
    foreachitemconst(hole, memView) { parBuffer.DrawLines(parMaterial, hole.data().data(), (u32)hole.size(), 0.0f, 0xFF0000FF, true); }

    const Navigation::FacesDataBase& faces = FNavMesh.Faces();
    foreachitemconst(face, faces)
    {
        const Navigation::NavMeshEdge* edge = face->Edge;
        std::vector<glm::vec2> faceVertices;
        do
        {
            faceVertices.push_back(edge->Vertex->Position);
            edge = edge->Next;
        } while (edge != face->Edge);
        parBuffer.DrawLines(parMaterial, faceVertices, faceVertices.size(), 0.f, 0xFFFF0000, true);
    }
}

void NavMeshPathfindingManager::ProcessOneRequest(const PathfindingRequest& parRequest)
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
    NavMeshPathfindingManager::CreateIFP();
}

void InitialisePathfinder(const Polygon2D& parWorldExtent, const std::vector<Polygon2D>& parObstacles)
{
    NavMeshPathfindingManager::Instance().Initialise(parWorldExtent, parObstacles);
}

void ModifyWorldExtents(const Polygon2D& parWorldExtent)
{
    NavMeshPathfindingManager::Instance().ModifyWorldExtents(parWorldExtent);
}

void AddObstacle(const Polygon2D& parObstacle)
{
    NavMeshPathfindingManager::Instance().AddObstacle(parObstacle);
}

void ModifyPathfinder(const Polygon2D& parWorldExtent, const std::vector<Polygon2D>& parObstacles)
{
}

void DestroyPathfinder()
{
    NavMeshPathfindingManager::Instance().Cleanup();
    NavMeshPathfindingManager::Destroy();
}

void PushRequest(PathfindingRequest&& parRequest)
{
    NavMeshPathfindingManager::Instance().PushRequest(std::move(parRequest));
}

PathfindingResult ComputeRequestSynchrone(PathfindingRequest&& parRequest)
{
    return NavMeshPathfindingManager::Instance().ComputeRequestsSynchrone(std::move(parRequest));
}

void ComputeRequests()
{
    NavMeshPathfindingManager::Instance().ComputeRequests();
}

void RetrieveResults(std::vector<PathfindingResult>& outResults)
{
    NavMeshPathfindingManager::Instance().RetrieveResults(outResults);
}

void Debug(Rendering::DrawCommandBuffer& parBuffer, const Rendering::MaterialInstanceHandle& parMaterial)
{
    NavMeshPathfindingManager::Instance().Debug(parBuffer, parMaterial);
}

} // namespace Pathfinding
} // namespace ECSEngine
