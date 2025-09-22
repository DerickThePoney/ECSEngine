#pragma once

namespace ECSEngine
{
namespace Physics
{

class PhysicsEngine;
class IslandManager
{
public:
    void SolveIslands(PhysicsEngine* Engine);
};

} // namespace Physics
} // namespace ECSEngine