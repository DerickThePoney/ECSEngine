#include "stdafx.h"

#include "IslandManager.h"

#include "Island.h"
#include "PhysicsEngine.h"

namespace ECSEngine
{
namespace Physics
{
void IslandManager::SolveIslands(PhysicsEngine* Engine)
{
    Island IslandToSolve;
    IslandToSolve.Init(Engine->FRigidbodies.size(), 0);
}
} // namespace Physics
} // namespace ECSEngine
