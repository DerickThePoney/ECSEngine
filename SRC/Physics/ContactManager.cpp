#include "stdafx.h"

#include "ContactManager.h"

#include "AABBTree.h"
#include "BroadPhase.h"

namespace ECSEngine
{
namespace Physics
{
IMPLEMENT_POOL_ALLOCATED(Contact);

void ContactManager::FindNewContacts(BroadPhase& parBroadPhase)
{
    FOverlapingPairDelegate Del = DELEGATE(&ContactManager::UpdateOverlapingPairsCallback, *this);
    parBroadPhase.UpdatePairs(Del);
}

void ContactManager::UpdateOverlapingPairsCallback(const PhysicsBodyHandle& first, const PhysicsBodyHandle& second)
{
}

} // namespace Physics
} // namespace ECSEngine
