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
}

void NavMesh::Cleanup()
{
    forrange(i, 0, FVertices.size()) { delete FVertices[i]; }
    FVertices.clear();

    forrange(i, 0, FVertices.size()) { delete FEdges[i]; }
    FEdges.clear();

    forrange(i, 0, FFaces.size()) { delete FFaces[i]; }
    FFaces.clear();
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

} // namespace Navigation
} // namespace ECSEngine