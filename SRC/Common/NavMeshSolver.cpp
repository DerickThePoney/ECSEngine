#include "stdafx.h"

#include "NavMeshSolver.h"

#include "NavMesh.h"
#include "Polygon.h"
#include "PolygonPartitionner.h"
#include "PolygonTriangulator.h"
#include "Triangle.h"

#include <glm/gtx/hash.hpp>

namespace ECSEngine
{
namespace Navigation
{
namespace
{
NavMeshVertex* GetOrCreateVertex(std::unordered_map<glm::vec2, NavMeshVertex*>& parVerticesMap, const glm::vec2 parVertex, bool& outWasCreated)
{
    NavMeshVertex* nmv = nullptr;
    auto itFind = parVerticesMap.find(parVertex);
    if (itFind != parVerticesMap.end())
    {
        nmv = itFind->second;
        outWasCreated = false;
    }
    else
    {
        nmv = new NavMeshVertex();
        nmv->Position = parVertex;
        parVerticesMap[parVertex] = nmv;
        outWasCreated = true;
    }

    AssertRelease(nmv != nullptr);
    return nmv;
}
NavMeshEdge* LookUpPotentialHalfEdge(const NavMeshVertex* start, const NavMeshVertex* end, const FacesDataBase& parFaces)
{
    foreachitemconst(face, parFaces)
    {
        NavMeshEdge* currentEdge = face->Edge;
        AssertRelease(currentEdge != nullptr);
        do
        {
            // if current vertex spans from end to start, terminate
            if (currentEdge->Vertex == end && currentEdge->Next->Vertex == start)
            {
                return currentEdge;
            }
            currentEdge = currentEdge->Next;
            AssertRelease(currentEdge != nullptr);
        } while (currentEdge != face->Edge);
    }
    return nullptr;
}
} // namespace

void NavMeshSolver::CreateNavMesh(const Polygon2D& parWorldExtents, const std::vector<Polygon2D>& parObstacles, NavMesh& outNavMesh)
{
    PolygonPartionner polyPart;
    std::vector<Polygon2D> partition = polyPart.Partition(parWorldExtents, parObstacles);
    AssertRelease(partition.size() > 0);

    std::unordered_map<glm::vec2, NavMeshVertex*> verticesMap;
    VerticesDataBase vertices;
    EdgesDataBase edges;
    FacesDataBase faces;
    faces.reserve(partition.size());

    // Count the number of edges
    u32 edgesCount = 0;
    foreachitemconst(polygon, partition) { edgesCount += (u32)polygon.size(); }
    edges.reserve(edgesCount);

    u32 faceId = 0;
    foreachitemconst(polygon, partition)
    {
        NavMeshFace* face = new NavMeshFace();
        face->FacePolygon = polygon;
        face->Id = faceId++;

        glm::vec2 center(0.f);

        std::vector<std::pair<NavMeshVertex*, bool>> verticesLoc;
        verticesLoc.resize(polygon.size());
        forrange(i, 0, (u32)polygon.size())
        {
            verticesLoc[i].first = GetOrCreateVertex(verticesMap, polygon[i], verticesLoc[i].second);
            center += polygon[i];
        }
        face->Center = center / polygon.size();

        std::vector<NavMeshEdge*> edgesLoc;
        edgesLoc.resize((u32)polygon.size());
        forrange(i, 0, (u32)polygon.size())
        {
            edgesLoc[i] = new NavMeshEdge();
            edges.push_back(edgesLoc[i]);

            // Connect vertex to edge
            verticesLoc[i].first->Edge.push_back(edgesLoc[i]);

            // Connect edge to vertex
            edgesLoc[i]->Vertex = verticesLoc[i].first;

            // Connect edge to face
            edgesLoc[i]->Face = face;
        }

        // Connect face to first edge
        face->Edge = edgesLoc[0];

        // Setup face connectivity
        forrange(i, 0, (u32)polygon.size())
        {
            const u32 next = ((u32)i + 1) % (u32)polygon.size();
            const u32 prev = ((u32)i == 0) ? ((u32)polygon.size() - 1u) : (u32)i - 1u;
            edgesLoc[i]->Next = edgesLoc[next];
            edgesLoc[i]->Prev = edgesLoc[prev];

            // lookup halfedge
            NavMeshEdge* potentialPair = LookUpPotentialHalfEdge(edgesLoc[i]->Vertex, edgesLoc[i]->Next->Vertex, faces);
            AssertRelease(potentialPair == nullptr || potentialPair->Pair == nullptr);

            if (potentialPair != nullptr)
            {
                potentialPair->Pair = edgesLoc[i];
                edgesLoc[i]->Pair = potentialPair;
            }
        }

        faces.push_back(face);
    }

    vertices.reserve(verticesMap.size());
    u32 index = 0;
    foreachitem(vertex, verticesMap)
    {
        vertex.second->Id = index++;
        vertices.push_back(vertex.second);
    }

    outNavMesh.Initialize(std::move(vertices), std::move(edges), std::move(faces), parWorldExtents, parObstacles);
}

} // namespace Navigation
} // namespace ECSEngine