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

void NavMesh::Initialize(VerticesDataBase&& parVertices,
      EdgesDataBase&& parEdges,
      FacesDataBase&& parFaces,
      const Polygon2D& parPolygon,
      const std::vector<Polygon2D>& parPolygonHoles)
{
    FVertices = parVertices;
    FEdges = parEdges;
    FFaces = parFaces;

    // Build Connected Component
    foreachitemconst(vertex, FVertices)
    {
        AssertRelease(FConnectedComponents.find(vertex->Id) == FConnectedComponents.end());
        NeighbourVerticesSet neighbours;
        FindNeighbourgingVertices(vertex, neighbours);
        FConnectedComponents.insert_or_assign(vertex->Id, neighbours);
    }

    FMainPolygon = parPolygon;
    FHoles = parPolygonHoles;
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
        if (Intersection::PointInPolygon2D(face->FacePolygon, parPoint))
        {
            return face;
        }
    }
    return nullptr;
}

const NeighbourVerticesSet& NavMesh::NeighbourVertices(const NavMeshVertex* parVertex) const
{
    AssertRelease(FConnectedComponents.find(parVertex->Id) != FConnectedComponents.end());
    return FConnectedComponents.at(parVertex->Id);
}

void NavMesh::NeighbourFaces(const NavMeshVertex* parVertex, NeighbourFacesSet& outFaces) const
{
    foreachitemconst(edge, parVertex->Edge) { outFaces.insert(edge->Face); }
}

void NavMesh::NeighbourFaces(const NavMeshFace* parFace, NeighbourFacesSet& outFaces) const
{
    NavMeshEdge* currentEdge = parFace->Edge;

    do
    {
        if (currentEdge->Pair != nullptr)
        {
            AssertRelease(currentEdge->Pair->Face != nullptr);
            outFaces.insert(currentEdge->Pair->Face);
        }
        currentEdge = currentEdge->Next;
    } while (currentEdge != parFace->Edge);
}

const NavMeshEdge* NavMesh::FindCommonEdge_AssumeExists(const NavMeshFace* parFace, const NavMeshFace* parOtherFace) const
{
    NavMeshEdge* currentEdge = parFace->Edge;

    do
    {
        if (currentEdge->Pair != nullptr && currentEdge->Pair->Face == parOtherFace)
        {
            return currentEdge;
        }
        currentEdge = currentEdge->Next;
    } while (currentEdge != parFace->Edge);

    AssertNotReached();
    return nullptr;
}

void NavMesh::FindNeighbourgingVertices(const NavMeshVertex* parVertex, NeighbourVerticesSet& outVertices) const
{
    std::set<NavMeshFace*> neighBouringFaces;
    NeighbourFaces(parVertex, neighBouringFaces);

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
                outVertices.insert(currentEdge->Vertex->Id);

            currentEdge = currentEdge->Next;
        } while (currentEdge != face->Edge);
    }
}

} // namespace Navigation
} // namespace ECSEngine
