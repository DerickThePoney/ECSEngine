#include "stdafx.h"

#include "Raycast.h"

#include "PhysicsEngine.h"

namespace ECSEngine
{
namespace Physics
{
bool RaycastClosest(const RaycastQuery& Query, RaycastHit& outHit)
{
    return PhysicsEngine::Instance().Raycast(Query, outHit);
}
} // namespace Physics
} // namespace ECSEngine
