#pragma once
#include "AABBTree.h"

namespace ECSEngine
{
namespace Physics
{
using OverlapingBodiesPair = std::pair<PhysicsBodyHandle, PhysicsBodyHandle>;
struct MovedBodies
{
    PhysicsBodyHandle Handle;
};

using FOverlapingPairDelegate = Delegate<void(const PhysicsBodyHandle&, const PhysicsBodyHandle&)>;

class BroadPhase
{
public:
    void UpdatePairs(FOverlapingPairDelegate& parCallback);

    void AddNewBody(RigidBody* body);
    void RemoveBody(RigidBody* body);

    void MoveBody(RigidBody* body, vec3 displacement);

    bool TestOverlap(const PhysicsBodyHandle& first, const PhysicsBodyHandle& second) const;

    AABB3f GetFatAABB3f(const PhysicsBodyHandle& handle) const;

#ifdef PERFORM_SECURITY_CHECKS
    void DebugBroadPhase();
#endif

private:
    void UpdatePotentialOverlappingPairs(FOverlapingPairDelegate& parCallback);
    void QueryCallback(const PhysicsBodyHandle& Handle);

private:
    AABBTree FTree;
    std::set<PhysicsBodyHandle> FMovedBodies;
    PhysicsBodyHandle FCurrentQuery;
    std::vector<OverlapingBodiesPair> FOverlapingBodies;
};
} // namespace Physics
} // namespace ECSEngine