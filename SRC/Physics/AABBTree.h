#pragma once
#include "Common/Ray.h"
#include "PhysicsBodyHandle.h"

namespace ECSEngine
{
namespace Physics
{
struct RigidBody;
struct AABBNode
{
    DECLARE_POOL_ALLOCATED(AABBNode);

public:
    AABB3f FAABB;
    PhysicsBodyHandle FBodyHandle;
    u32 FParentIndex = -1u;
    u32 FChild1 = -1u;
    u32 FChild2 = -1u;
    i32 FHeight = -1;
    bool FbIsLeaf = false;
    bool bWasMoved = false;

    void Reset();
};

using FQueryCallback = Delegate<void(const PhysicsBodyHandle&)>;

class AABBTree
{
public:
    ~AABBTree();

    void Clear();

    void InsertBody(const RigidBody* Body);
    void RemoveBody(const RigidBody* Body);

    bool MoveBody(const RigidBody* Body, vec3 parDisplacement);
    bool WasMoved(const PhysicsBodyHandle& parHandle) const;
    void ClearMoved(const PhysicsBodyHandle& parHandle);

    void RaycastTree(const Ray3D& parRay, std::vector<PhysicsBodyHandle>& parLeafsHit);

    void OverlapQuery(const AABB3f& parAABB, const FQueryCallback& Callback);

    AABB3f GetFatAABB3f(const PhysicsBodyHandle& parHandle) const;

#ifdef PERFORM_SECURITY_CHECKS
    void DebugTree();
#endif

private:
    void InsertLeaf(const u32 Index);
    void RemoveLeaf(const u32 Index);
    u32 AllocateNewLeaf(const RigidBody* Body);
    u32 AllocateNewNode();
    void FreeNode(const u32 Index);
    u32 FindBestSibling(const u32 NewLeafIndex) const;
    u32 BalanceNode(const u32 NodeIndex);
    float SurfaceArea(const AABB3f& Node) const;
    mat4 ComputeBodyTransform(const RigidBody* Body) const;
    AABB3f ComputeAABB(const RigidBody* Body) const;
    AABB3f ComputeAABB(const RigidBody* Body, const mat4& Transform) const;
    u32 RetrieveNodeForBody(const RigidBody* Body) const;
    u32 RetrieveNodeForHandle(const PhysicsBodyHandle& BodyHandle) const;

private:
    std::vector<AABBNode*> FNodes;
    std::set<u32> FFreeNodeIndices;

    std::map<PhysicsBodyHandle, u32> FHandleToNodeMap;

    u32 FRootIndex = -1u;
};
} // namespace Physics
} // namespace ECSEngine
