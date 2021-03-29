#pragma once

namespace ECSEngine
{
class Polygon2D;

namespace Navigation
{
class NavMesh;
class NavMeshSolver
{
public:
    NavMeshSolver() { }

    void CreateNavMesh(const Polygon2D& parWorldExtents, const std::vector<Polygon2D>& parObstacles, NavMesh& outNavMesh);
};
} // namespace Navigation
} // namespace ECSEngine
