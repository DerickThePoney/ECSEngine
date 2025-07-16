#include "stdafx.h"

#include "BroadPhase.h"

#include "Common/IntersectionRoutines.h"
#include "RigidBody.h"

namespace ECSEngine
{
namespace Physics
{

void BroadPhase::Shutdown()
{
    FTree.Clear();
    FMovedBodies.clear();
    FCurrentQuery = PhysicsBodyHandle{};
    FOverlapingBodies.clear();
}

void BroadPhase::UpdatePairs(FOverlapingPairDelegate& parCallback)
{
    UpdatePotentialOverlappingPairs(parCallback);
}

void BroadPhase::AddNewBody(RigidBody* body)
{
    FTree.InsertBody(body);
    FMovedBodies.insert(body->FHandle);
}

void BroadPhase::RemoveBody(RigidBody* body)
{
    FTree.RemoveBody(body);
    FMovedBodies.erase(body->FHandle);
}

void BroadPhase::MoveBody(RigidBody* body, vec3 displacement)
{
    const bool bBufferMove = FTree.MoveBody(body, displacement);
    if (bBufferMove)
    {
        FMovedBodies.insert(body->FHandle);
    }
}

bool BroadPhase::TestOverlap(const PhysicsBodyHandle& first, const PhysicsBodyHandle& second) const
{
    const AABB3f FirstAABB = FTree.GetFatAABB3f(first);
    const AABB3f SecondAABB = FTree.GetFatAABB3f(second);
    return Intersection::AABBABBBIntersection(FirstAABB, SecondAABB);
}

AABB3f BroadPhase::GetFatAABB3f(const PhysicsBodyHandle& handle) const
{
    return FTree.GetFatAABB3f(handle);
}

void BroadPhase::UpdatePotentialOverlappingPairs(FOverlapingPairDelegate& parCallback)
{
    // Query tree for each moved object - get add a call back to record the pairs
    // Loop over overlapping pairs
    // clear the moved flags.
    FOverlapingBodies.clear(); // Necessary ?

    foreachitemconst(bodyHandle, FMovedBodies)
    {
        FCurrentQuery = bodyHandle;
        const AABB3f AABBToUse = FTree.GetFatAABB3f(bodyHandle);
        FTree.OverlapQuery(AABBToUse, DELEGATE(&BroadPhase::QueryCallback, *this));
    }

    // Send data upstage
    foreachitemconst(OverlapingBodies, FOverlapingBodies)
    {
        parCallback(OverlapingBodies.first, OverlapingBodies.second);
    }

    // clear moved flags and buffer
    foreachitemconst(bodyHandle, FMovedBodies)
    {
        FTree.ClearMoved(bodyHandle);
    }
    FMovedBodies.clear();
}

void BroadPhase::QueryCallback(const PhysicsBodyHandle& Handle)
{
    if (FCurrentQuery == Handle)
    {
        return;
    }

    if (FTree.WasMoved(Handle) && Handle.FId > FCurrentQuery.FId)
    {
        return;
    }

    // Moved buffer
    OverlapingBodiesPair newPair;
    newPair.first = (FCurrentQuery.FId < Handle.FId) ? FCurrentQuery : Handle;
    newPair.second = (FCurrentQuery.FId < Handle.FId) ? Handle : FCurrentQuery;
    FOverlapingBodies.push_back(newPair);
}

#ifdef PERFORM_SECURITY_CHECKS
void BroadPhase::DebugBroadPhase()
{
    FTree.DebugTree();
}
#endif

} // namespace Physics
} // namespace ECSEngine