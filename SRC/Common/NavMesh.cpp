#include "stdafx.h"

#include "NavMesh.h"

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

} // namespace Navigation
} // namespace ECSEngine