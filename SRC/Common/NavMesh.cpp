#include "stdafx.h"

#include "NavMesh.h"

#include "IntersectionRoutines.h"
#include "Triangle.h"

namespace ECSEngine
{
namespace Navigation
{
IMPLEMENT_POOL_ALLOCATED(NavMeshVertex);
IMPLEMENT_POOL_ALLOCATED(NavMeshEdge);
IMPLEMENT_POOL_ALLOCATED(NavMeshFace);

void NavMesh::Initialize(VerticesDataBase&& parVertices, EdgesDataBase&& parEdges, FacesDataBase&& parFaces)
{
    FVertices = parVertices;
    FEdges = parEdges;
    FFaces = parFaces;

    // Build Connected Component
    foreachitemconst(vertex, FVertices)
    {
        AssertRelease(FConnectedComponents.find(vertex) == FConnectedComponents.end());
        NeighbourVerticesSet neighbours;
        FindNeighbourgingVertices(vertex, neighbours);
        FConnectedComponents.insert_or_assign(vertex, neighbours);
    }
}

void NavMesh::Cleanup()
{
    forrange(i, 0, FVertices.size()) { delete FVertices[i]; }
    FVertices.clear();

    forrange(i, 0, FVertices.size()) { delete FEdges[i]; }
    FEdges.clear();

    forrange(i, 0, FFaces.size()) { delete FFaces[i]; }
    FFaces.clear();

    FConnectedComponents.clear();
}

const NavMeshFace* NavMesh::FindContainingFace(const glm::vec2 parPoint) const
{
    foreachitemconst(face, FFaces)
    {
        Triangle2D faceTri;
        faceTri.A = face->Edge->Vertex->Position;
        faceTri.B = face->Edge->Next->Vertex->Position;
        faceTri.C = face->Edge->Next->Next->Vertex->Position;

        if (Intersection::PointInTriangle2D(faceTri, parPoint))
        {
            return face;
        }
    }
    return nullptr;
}

const NeighbourVerticesSet& NavMesh::GetNeighbours(NavMeshVertex* parVertex) const
{
    AssertRelease(FConnectedComponents.find(parVertex) != FConnectedComponents.end());
    return FConnectedComponents.at(parVertex);
}

void NavMesh::FindNeighbourgingVertices(const NavMeshVertex* parVertex, std::set<NavMeshVertex*>& outVertices) const
{
    std::set<NavMeshFace*> neighBouringFaces;

    foreachitemconst(edge, parVertex->Edge) { neighBouringFaces.insert(edge->Face); }

    foreachitemconst(face, neighBouringFaces)
    {
        NavMeshEdge* currentEdge = face->Edge;
        do
        {
            if (currentEdge->Vertex == parVertex)
            {
                currentEdge = currentEdge->Next;
                continue;
            }

            if (currentEdge->Next->Vertex == parVertex || currentEdge->Prev->Vertex == parVertex)
                outVertices.insert(currentEdge->Vertex);

            currentEdge = currentEdge->Next;
        } while (currentEdge != face->Edge);
    }
}

} // namespace Navigation
} // namespace ECSEngine