#pragma once

namespace ECSEngine
{
namespace Navigation
{
class NavMesh;
class NavMeshPath;
class NavMeshPathSmoother
{
public:
    void SmoothPath(const NavMesh& parNavMesh, NavMeshPath& outPath);
};
} // namespace Navigation
} // namespace ECSEngine