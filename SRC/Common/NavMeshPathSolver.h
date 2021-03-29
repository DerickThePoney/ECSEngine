#pragma once

namespace ECSEngine
{
namespace Navigation
{
class NavMesh;
class NavMeshPath;
class NavMeshPathSolver
{
public:
    NavMeshPathSolver() { }
    ~NavMeshPathSolver() { }

    void SolvePath(const NavMesh& parNavMesh, NavMeshPath& outPath);

private:
    void SolveVertexPath(const NavMesh& parNavMesh, NavMeshPath& outPath);
    void SolveFacePath(const NavMesh& parNavMesh, NavMeshPath& outPath);
};
} // namespace Navigation
} // namespace ECSEngine
