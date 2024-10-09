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
    parBroadPhase.UpdatePairs();
}

} // namespace Physics
} // namespace ECSEngine
