#pragma once
#include "MemoryView.h"
#include "Polygon.h"
#include "PoolAllocator.h"

namespace ECSEngine
{
namespace Navigation
{
struct NavMeshEdge;
struct NavMeshFace;
struct NavMeshVertex
{
    DECLARE_POOL_ALLOCATED(NavMeshVertex);

public:
    glm::vec2 Position = glm::vec2(0.f);
    std::vector<NavMeshEdge*> Edge;
    u32 Id = 0;
};

struct NavMeshEdge
{
    DECLARE_POOL_ALLOCATED(NavMeshEdge);

public:
    NavMeshVertex* Vertex = nullptr;
    NavMeshEdge* Pair = nullptr;
    NavMeshFace* Face = nullptr;
    NavMeshEdge* Next = nullptr;
    NavMeshEdge* Prev = nullptr;
};

struct NavMeshFace
{
    DECLARE_POOL_ALLOCATED(NavMeshFace);

public:
    NavMeshEdge* Edge = nullptr;
    Polygon2D FacePolygon;
    glm::vec2 Center;
    u32 Id;
};

using VerticesDataBase = std::vector<NavMeshVertex*>;
using NeighbourVerticesSet = std::set<u32>;
using NeighbourFacesSet = std::set<NavMeshFace*>;
using ConnectedComponents = std::unordered_map<u32, NeighbourVerticesSet>;
using EdgesDataBase = std::vector<NavMeshEdge*>;
using FacesDataBase = std::vector<NavMeshFace*>;
class NavMesh
{
public:
    NavMesh() { }
    ~NavMesh() { Cleanup(); }

    void Initialize(VerticesDataBase&& parVertices, EdgesDataBase&& parEdges, FacesDataBase&& parFaces, const Polygon2D& parPolygon, const std::vector<Polygon2D>& parPolygonHoles);
    void Cleanup();

    const VerticesDataBase& Vertices() const { return FVertices; }
    const EdgesDataBase& Edges() const { return FEdges; }
    const FacesDataBase& Faces() const { return FFaces; }

    u32 VerticesCount() const { return (u32)FVertices.size(); }
    u32 EdgesCount() const { return (u32)FEdges.size(); }
    u32 FacesCount() const { return (u32)FFaces.size(); }

    const NavMeshFace* FindContainingFace(const glm::vec2 parPoint) const;
    const NeighbourVerticesSet& NeighbourVertices(const NavMeshVertex* parVertex) const;

    void NeighbourFaces(const NavMeshVertex* parVertex, NeighbourFacesSet& outFaces) const;
    void NeighbourFaces(const NavMeshFace* parFace, NeighbourFacesSet& outFaces) const;
    const NavMeshEdge* FindCommonEdge_AssumeExists(const NavMeshFace* parFace, const NavMeshFace* parOtherFace) const;

    const Polygon2D& MainPolygon() const { return FMainPolygon; }
    const MemoryView<const Polygon2D> Holes() const { return MemoryView<const Polygon2D>(FHoles.data(), (u32)FHoles.size()); }

private:
    void FindNeighbourgingVertices(const NavMeshVertex* parVertex, NeighbourVerticesSet& outVertices) const;

private:
    VerticesDataBase FVertices;
    EdgesDataBase FEdges;
    FacesDataBase FFaces;
    ConnectedComponents FConnectedComponents;

    Polygon2D FMainPolygon;
    std::vector<Polygon2D> FHoles;
};
} // namespace Navigation
} // namespace ECSEngine