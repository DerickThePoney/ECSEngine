#pragma once

namespace ECSEngine
{
namespace Physics
{

class PhysicsEngine;
class IslandManager
{
public:
    void SolveIslands(PhysicsEngine* Engine, float parDeltaTime);
};

} // namespace Physics
} // namespace ECSEngine