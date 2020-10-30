#include "stdafx.h"

#include "NavMeshPathSolver.h"

#include "NavMesh.h"
#include "NavMeshPath.h"

namespace ECSEngine
{
namespace Navigation
{

namespace PathfindingHelpers
{
float Distance(const glm::vec2 parVertexPos, const glm::vec2 parLastVertexPos)
{
    return glm::length2(parVertexPos - parLastVertexPos);
}

float Heuristic(const glm::vec2 parVertexPos, const glm::vec2 parGoal)
{
    return Distance(parGoal, parVertexPos);
}

float Score(const float parDistance, const float parHeuristic)
{
    return parDistance + parHeuristic;
}
} // namespace PathfindingHelpers

struct PathFindNode
{
    float Score() const { return PathfindingHelpers::Score(DistanceSoFar, HeuristicScore); }

    bool operator<(const PathFindNode& parOther) const { return Score() < parOther.Score(); }
    bool operator<=(const PathFindNode& parOther) const { return Score() <= parOther.Score(); }
    bool operator>(const PathFindNode& parOther) const { return Score() > parOther.Score(); }
    bool operator>=(const PathFindNode& parOther) const { return Score() >= parOther.Score(); }

    float DistanceSoFar = std::numeric_limits<float>::max();
    float HeuristicScore = std::numeric_limits<float>::max();
    u32 Id = -1;
    u32 From = -1;
};
static_assert(std::is_trivially_copyable<PathFindNode>(), "PathFindNode must be trivially copyable!!");

void NavMeshPathSolver::SolvePath(const NavMesh& parNavMesh, NavMeshPath& outPath)
{
    const glm::vec2 start = outPath.Start();
    const NavMeshFace* startFace = parNavMesh.FindContainingFace(start);
    if (startFace == nullptr)
        return;

    const glm::vec2 end = outPath.End();
    const NavMeshFace* endFace = parNavMesh.FindContainingFace(end);
    if (endFace == nullptr)
        return;

    // same face -> we're good stop there
    if (startFace == endFace)
    {
        outPath.SetValid(true);
        return;
    }

    // Otherwise, navigate the mesh
    const VerticesDataBase& vertices = parNavMesh.Vertices();
    std::vector<PathFindNode> pathFindingNodes(vertices.size());
    forrange(i, 0, vertices.size())
    {
        pathFindingNodes[i].HeuristicScore = PathfindingHelpers::Heuristic(vertices[i]->Position, end);
        pathFindingNodes[i].Id = (u32)i;
    }
    std::priority_queue<PathFindNode, std::vector<PathFindNode>, std::greater<PathFindNode>> openList;

    // push the nodes of the start face onto the open list
    NavMeshEdge* currentEdge = startFace->Edge;
    do
    {
        pathFindingNodes[currentEdge->Vertex->Id].DistanceSoFar = PathfindingHelpers::Distance(currentEdge->Vertex->Position, start);
        openList.push(pathFindingNodes[currentEdge->Vertex->Id]);
        currentEdge = currentEdge->Next;
    } while (currentEdge != startFace->Edge);

    u32 lastVertex = -1;
    bool success = false;
    while (!openList.empty())
    {
        const PathFindNode current = openList.top();
        openList.pop();

        lastVertex = current.Id;

        // check connected faces then break
        NeighbourFacesSet neighbouringFaces;
        parNavMesh.NeighbourFaces(vertices[lastVertex], neighbouringFaces);
        foreachitemconst(face, neighbouringFaces)
        {
            // On est concomitant à la face finale, yeah!!
            if (face == endFace)
            {
                success = true;
                break;
            }
        }

        if (success)
            break;

        // loop neighbours
        const NeighbourVerticesSet& neighbours = parNavMesh.NeighbourVertices(vertices[lastVertex]);
        foreachitemconst(neighbour, neighbours)
        {
            const float distanceSoFar = PathfindingHelpers::Distance(vertices[neighbour]->Position, vertices[lastVertex]->Position) + current.DistanceSoFar;
            const float score = PathfindingHelpers::Score(distanceSoFar, pathFindingNodes[neighbour].HeuristicScore);

            if (score < pathFindingNodes[neighbour].Score())
            {
                // update the node, and push it onto the open list
                pathFindingNodes[neighbour].From = lastVertex;
                pathFindingNodes[neighbour].DistanceSoFar = distanceSoFar;
                openList.push(pathFindingNodes[neighbour]);
            }
        }
    }

    if (!success)
        return;

    // reconstruct path
    std::vector<u32> path;
    path.reserve(vertices.size());

    while (lastVertex != -1)
    {
        path.push_back(lastVertex);
        lastVertex = pathFindingNodes[lastVertex].From;
    }

    // fill out the path in reserve
    outPath.SetValid(true);
    outPath.reserve(path.size());
    reverseforrange(i, 0, path.size()) { outPath.push_back(vertices[path[i]]); }
}

} // namespace Navigation
} // namespace ECSEngine