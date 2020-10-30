#include "stdafx.h"

#include "NavMeshSolver.h"

#include "NavMesh.h"
#include "Polygon.h"
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
    PolygonTriangulator polyTri;
    std::vector<Triangle2D> triangulation = polyTri.Triangulate(parWorldExtents, parObstacles);
    AssertRelease(triangulation.size() > 0);

    std::unordered_map<glm::vec2, NavMeshVertex*> verticesMap;
    VerticesDataBase vertices;
    EdgesDataBase edges;
    FacesDataBase faces;
    faces.reserve(triangulation.size());
    edges.reserve(triangulation.size() * 3);

    foreachitemconst(triangle, triangulation)
    {
        NavMeshFace* face = new NavMeshFace();

        bool v1New = false, v2New = false, v3New = false;
        NavMeshVertex* v1 = GetOrCreateVertex(verticesMap, triangle.A, v1New);
        NavMeshVertex* v2 = GetOrCreateVertex(verticesMap, triangle.B, v2New);
        NavMeshVertex* v3 = GetOrCreateVertex(verticesMap, triangle.C, v3New);

        NavMeshEdge* e1 = new NavMeshEdge();
        edges.push_back(e1);
        NavMeshEdge* e2 = new NavMeshEdge();
        edges.push_back(e2);
        NavMeshEdge* e3 = new NavMeshEdge();
        edges.push_back(e3);

        // Connect vertices and edges
        v1->Edge.push_back(e1);
        v2->Edge.push_back(e2);
        v3->Edge.push_back(e3);

        // Connect face to first edge
        face->Edge = e1;

        // Connect edges to face
        e1->Face = face;
        e2->Face = face;
        e3->Face = face;

        // Setup face connectivity
        e1->Next = e2;
        e2->Next = e3;
        e3->Next = e1;

        e1->Prev = e3;
        e2->Prev = e1;
        e3->Prev = e2;

        // Connect edge to vertices
        e1->Vertex = v1;
        e2->Vertex = v2;
        e3->Vertex = v3;

        // Lookup half edge
        NavMeshEdge* e1Pair = LookUpPotentialHalfEdge(v1, v2, faces);
        NavMeshEdge* e2Pair = LookUpPotentialHalfEdge(v2, v3, faces);
        NavMeshEdge* e3Pair = LookUpPotentialHalfEdge(v3, v1, faces);

        AssertRelease(e1Pair == nullptr || e1Pair->Pair == nullptr);
        AssertRelease(e2Pair == nullptr || e2Pair->Pair == nullptr);
        AssertRelease(e3Pair == nullptr || e3Pair->Pair == nullptr);

        // Connect halfedges
        if (e1Pair != nullptr)
        {
            e1Pair->Pair = e1;
            e1->Pair = e1Pair;
        }

        if (e2Pair != nullptr)
        {
            e2Pair->Pair = e2;
            e2->Pair = e2Pair;
        }

        if (e3Pair != nullptr)
        {
            e3Pair->Pair = e3;
            e3->Pair = e3Pair;
        }

        faces.push_back(face);
    }

    vertices.reserve(verticesMap.size());
    foreachitemconst(vertex, verticesMap) { vertices.push_back(vertex.second); }

    outNavMesh.Initialize(std::move(vertices), std::move(edges), std::move(faces));
}

} // namespace Navigation
} // namespace ECSEngine