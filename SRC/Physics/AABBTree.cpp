#include "stdafx.h"

#include "AABBTree.h"

#include "Common/IntersectionRoutines.h"
#include "GeometryHelpers.h"
#include "PhysicsAPI.h"
#include "PhysicsEngineConfiguration.h"
#include "RigidBody.h"

#ifdef PERFORM_SECURITY_CHECKS
#include "Common/CameraHelpers.h"
#include "Common/CameraManager.h"
#include "Common/ColorUtils.h"
#include "Common/InputManager.h"
#include "ECSCore/AdjustableDebugParameters.h"
#include "RenderingCore/BGFXRenderingBackend.h"
#include "RenderingCore/DrawCommands.h"
#include "RenderingCore/GLFWDisplayWindowHandler.h"
#include "RenderingCore/MaterialManager.h"
#endif

namespace ECSEngine
{
namespace Physics
{
IMPLEMENT_POOL_ALLOCATED(AABBNode);

AABBTree::~AABBTree()
{
    Clear();
}

void AABBTree::Clear()
{
    foreachitem(node, FNodes)
    {
        delete node;
    }
}

void AABBTree::InsertBody(const RigidBody* Body)
{
    u32 newLeafIndex = AllocateNewLeaf(Body);
    InsertLeaf(newLeafIndex);
}

void AABBTree::RemoveBody(const RigidBody* Body)
{
    auto itFind = FHandleToNodeMap.find(Body->FHandle);
    AssertRelease(itFind != FHandleToNodeMap.end());
    AssertRelease(itFind->second < FNodes.size());
    AssertRelease(itFind->first == FNodes[itFind->second]->FBodyHandle);
    AssertRelease(FNodes[itFind->second]->FbIsLeaf);
    u32 Index = itFind->second;
    FHandleToNodeMap.erase(itFind);
    RemoveLeaf(Index);
    FreeNode(Index);
}

bool AABBTree::MoveBody(const RigidBody* Body, vec3 parDisplacement)
{
    const u32 nodeIndex = RetrieveNodeForBody(Body);
    AlwaysCheckedAssert(nodeIndex != -1u);
    if (nodeIndex == -1u)
    {
        return false;
    }

    const PhysicsEngineConfiguration& Config = Physics::GetConfig();

    // Compute the new AABB
    AABB3f aabb = ComputeAABB(Body);
    const float fatteningValue = Config.FAABBFattenValue;

    AABB3f fatAabb = aabb;
    fatAabb.Inflate(fatteningValue);

    // Extend it in the direction of movement
    const vec3 extendedDisplacement = Config.FAABBDisplacementMultiplier * parDisplacement;

    if (extendedDisplacement.x < 0.f)
    {
        fatAabb.Min().x += extendedDisplacement.x;
    }
    else
    {
        fatAabb.Max().x += extendedDisplacement.x;
    }

    if (extendedDisplacement.y < 0.f)
    {
        fatAabb.Min().y += extendedDisplacement.y;
    }
    else
    {
        fatAabb.Max().y += extendedDisplacement.y;
    }

    if (extendedDisplacement.z < 0.f)
    {
        fatAabb.Min().z += extendedDisplacement.z;
    }
    else
    {
        fatAabb.Max().z += extendedDisplacement.z;
    }

    const AABB3f& treeAABB = FNodes[nodeIndex]->FAABB;
    if (GeometryHelpers::FirstAABBContainsSecond(treeAABB, aabb))
    {
        // The node AABB contains the new aabb -> check if we need to update the tree to shrink the AABB anyway
        // Inflate fatAABB again
        AABB3f hugeAABB = fatAabb;
        hugeAABB.Inflate(fatteningValue * 4.f);
        if (GeometryHelpers::FirstAABBContainsSecond(hugeAABB, treeAABB))
        {
            return false;
        }
        // tree AABB is too big it needs to shrink
    }

    RemoveLeaf(nodeIndex);

    FNodes[nodeIndex]->FAABB = fatAabb;

    InsertLeaf(nodeIndex);
    FNodes[nodeIndex]->bWasMoved = true;

    return true;
}

bool AABBTree::WasMoved(const PhysicsBodyHandle& parHandle) const
{
    const u32 Index = RetrieveNodeForHandle(parHandle);
    AssertRelease(Index < FNodes.size());
    AssertRelease(FNodes[Index] != nullptr);
    return FNodes[Index]->bWasMoved;
}

void AABBTree::ClearMoved(const PhysicsBodyHandle& parHandle)
{
    const u32 Index = RetrieveNodeForHandle(parHandle);
    AssertRelease(Index < FNodes.size());
    AssertRelease(FNodes[Index] != nullptr);
    FNodes[Index]->bWasMoved = false;
}

void AABBTree::RaycastTree(const Ray3D& parRay, std::vector<PhysicsBodyHandle>& parLeafsHit)
{
    std::queue<u32> nodesQueue;

    if (FRootIndex == -1u)
    {
        return;
    }

    nodesQueue.push(FRootIndex);
    while (!nodesQueue.empty())
    {
        const u32 currentNodeIndex = nodesQueue.front();
        nodesQueue.pop();
        AssertRelease(currentNodeIndex != -1u);

        AABBNode* currentNode = FNodes[currentNodeIndex];
        AssertRelease(currentNode != nullptr);
        if (!Intersection::RayAABBIntersection(parRay, currentNode->FAABB))
        {
            continue;
        }

        if (currentNode->FbIsLeaf)
        {
            parLeafsHit.push_back(currentNode->FBodyHandle);
        }
        else
        {
            if (currentNode->FChild1 != -1u)
                nodesQueue.push(currentNode->FChild1);

            if (currentNode->FChild2 != -1u)
                nodesQueue.push(currentNode->FChild2);
        }
    }
}

void AABBTree::OverlapQuery(const AABB3f& parAABB, FQueryCallback& Callback)
{
    std::queue<u32> nodesQueue;

    if (FRootIndex == -1u)
    {
        return;
    }

    nodesQueue.push(FRootIndex);
    while (!nodesQueue.empty())
    {
        const u32 currentNodeIndex = nodesQueue.front();
        AssertRelease(currentNodeIndex != -1u);
        nodesQueue.pop();

        AABBNode* currentNode = FNodes[currentNodeIndex];
        AssertRelease(currentNode != nullptr);
        if (!Intersection::AABBABBBIntersection(parAABB, currentNode->FAABB))
        {
            continue;
        }

        if (currentNode->FbIsLeaf)
        {
            Callback(currentNode->FBodyHandle);
        }
        else
        {
            if (currentNode->FChild1 != -1u)
                nodesQueue.push(currentNode->FChild1);

            if (currentNode->FChild2 != -1u)
                nodesQueue.push(currentNode->FChild2);
        }
    }
}

AABB3f AABBTree::GetFatAABB3f(const PhysicsBodyHandle& parHandle) const
{
    AssertRelease(parHandle.IsValid());
    auto itFind = FHandleToNodeMap.find(parHandle);
    AssertRelease(itFind != FHandleToNodeMap.end());
    AssertRelease(itFind->second < FNodes.size());

    const AABBNode* const Node = FNodes[itFind->second];
    AssertRelease(Node != nullptr);
    AssertRelease(Node->FbIsLeaf);
    return Node->FAABB;
}

#ifdef PERFORM_SECURITY_CHECKS
void AABBTree::DebugTree()
{
    ADJUSTABLE_DEBUG_PARAMETER_BOOLEAN(bShowAABBTreeAABBs, false, "Show AABB Tree", "Physics/AABBTree");
    ADJUSTABLE_DEBUG_PARAMETER_BOOLEAN(bOnlyDrawLeaf, false, "Only show leafs", "Physics/AABBTree");
    ADJUSTABLE_DEBUG_PARAMETER_BOOLEAN(bDebugAABBTreeRaycast, false, "Debug AABB Tree Raycast", "Physics/AABBTree");
    if (bShowAABBTreeAABBs)
    {
        Rendering::DrawCommandBuffer* buffer = Rendering::BGFXRenderingBackend::Instance().CreateCommandBuffer(Rendering::RenderPassId::DEBUG_PASS);
        u32 camId = CameraManager::Instance().CreateCameraIFN("GameplayCamera");
        Camera* camera = CameraManager::Instance().GetCamera(camId);
        buffer->SetViewTranform(camera->GetWorldViewMatrix(), camera->GetProjectionMatrix(Rendering::GLFWDisplayWindowHandler::Instance().AspectRatio()));
        Rendering::MaterialInstanceHandle handle = Rendering::MaterialManager::CreateMaterialInstanceIFN("materials\\vertexcolormaterial.material");

        static const u32 LeafColor = ColorUtils::ConvertToU32(vec4(0.f, 1.f, 0.f, 1.f));
        static const u32 RootColor = ColorUtils::ConvertToU32(vec4(1.f, 0.f, 0.f, 1.f));

        foreachitemconst(node, FNodes)
        {
            if (node == nullptr)
            {
                continue;
            }

            if (bOnlyDrawLeaf && !node->FbIsLeaf)
            {
                continue;
            }

            u32 color = ColorUtils::ConvertToU32(vec4(1.f, 0.f, 1.f, 1.f));
            if (node->FbIsLeaf)
                color = LeafColor;
            if (node->FParentIndex == -1u)
                color = RootColor;

            buffer->DrawAABB(handle, node->FAABB.Min(), node->FAABB.Max(), color);
        }

        buffer->Submit();
        Rendering::BGFXRenderingBackend::Instance().ReleaseCommandBuffer(buffer);
    }

    if (bDebugAABBTreeRaycast)
    {
        u32 camId = CameraManager::Instance().CreateCameraIFN("GameplayCamera");
        Camera* camera = CameraManager::Instance().GetCamera(camId);

        const uvec2 windowSize = Rendering::GLFWDisplayWindowHandler::Instance().GetSize();
        const float aspectRatio = Rendering::GLFWDisplayWindowHandler::Instance().AspectRatio();
        Ray ray = GetCameraRayFromMouseInput(*camera, aspectRatio, windowSize, Input::GetMousePosition());
        std::vector<PhysicsBodyHandle> Hits;
        RaycastTree(ray, Hits);

        Rendering::DrawCommandBuffer* buffer = Rendering::BGFXRenderingBackend::Instance().CreateCommandBuffer(Rendering::RenderPassId::DEBUG_PASS);
        buffer->SetViewTranform(camera->GetWorldViewMatrix(), camera->GetProjectionMatrix(aspectRatio));
        Rendering::MaterialInstanceHandle handle = Rendering::MaterialManager::CreateMaterialInstanceIFN("materials\\vertexcolormaterial.material");

        for (PhysicsBodyHandle& Handle : Hits)
        {
            auto itFind = FHandleToNodeMap.find(Handle);
            if (itFind == FHandleToNodeMap.end())
            {
                continue;
            }

            AABBNode* HitNode = FNodes[itFind->second];
            if (HitNode == nullptr)
            {
                continue;
            }

            buffer->DrawAABB(handle, HitNode->FAABB.Min(), HitNode->FAABB.Max(), ColorUtils::FromRGBA(255, 0, 0, 255));
        }
        buffer->Submit();
        Rendering::BGFXRenderingBackend::Instance().ReleaseCommandBuffer(buffer);
    }
}
#endif

void AABBTree::InsertLeaf(const u32 Index)
{
    if (FRootIndex == -1u)
    {
        FRootIndex = Index;
        return;
    }

    // 1 - Find the best sibling for the shape
    const u32 bestSiblingToAddTo = FindBestSibling(Index);

    // 2 - create new FParentIndex
    u32 newParentNodeIndex = AllocateNewNode();
    AABBNode* NewParentNode = FNodes[newParentNodeIndex];
    NewParentNode->FParentIndex = FNodes[bestSiblingToAddTo]->FParentIndex;
    NewParentNode->FAABB = AABB3f::Union(FNodes[bestSiblingToAddTo]->FAABB, FNodes[Index]->FAABB);
    NewParentNode->FChild1 = bestSiblingToAddTo;
    NewParentNode->FChild2 = Index;
    NewParentNode->FHeight = FNodes[bestSiblingToAddTo]->FHeight + 1;
    FNodes[bestSiblingToAddTo]->FParentIndex = newParentNodeIndex;
    FNodes[Index]->FParentIndex = newParentNodeIndex;
    FNodes[Index]->bWasMoved = true;

    if (NewParentNode->FParentIndex == -1u)
    {
        // sibling was the root
        FRootIndex = newParentNodeIndex;
    }
    else
    {
        // sibling was NOT the root
        if (FNodes[NewParentNode->FParentIndex]->FChild1 == bestSiblingToAddTo)
        {
            FNodes[NewParentNode->FParentIndex]->FChild1 = newParentNodeIndex;
        }
        else
        {
            FNodes[NewParentNode->FParentIndex]->FChild2 = newParentNodeIndex;
        }
    }

    // 3 - Walk back up the chain - taken straight from Box2d's code
    u32 CurrentNodeToLookAt = newParentNodeIndex;
    while (CurrentNodeToLookAt != -1u)
    {
        // Balance the tree
        CurrentNodeToLookAt = BalanceNode(CurrentNodeToLookAt);

        // refit the AABBs
        i32 child1 = FNodes[CurrentNodeToLookAt]->FChild1;
        i32 child2 = FNodes[CurrentNodeToLookAt]->FChild2;

        AssertRelease(child1 != -1u);
        AssertRelease(child2 != -1u);

        FNodes[CurrentNodeToLookAt]->FHeight = 1 + std::max(FNodes[child1]->FHeight, FNodes[child2]->FHeight);
        FNodes[CurrentNodeToLookAt]->FAABB = AABB3f::Union(FNodes[FNodes[CurrentNodeToLookAt]->FChild1]->FAABB, FNodes[FNodes[CurrentNodeToLookAt]->FChild2]->FAABB);
        CurrentNodeToLookAt = FNodes[CurrentNodeToLookAt]->FParentIndex;
    }
}

void AABBTree::RemoveLeaf(const u32 Index)
{
    if (Index == FRootIndex)
    {
        FRootIndex = -1u;
        return;
    }

    AABBNode* Node = FNodes[Index];
    const u32 parent = Node->FParentIndex;
    const u32 grandParent = FNodes[Node->FParentIndex]->FParentIndex;
    u32 sibling;
    if (FNodes[Node->FParentIndex]->FChild1 == Index)
    {
        sibling = FNodes[Node->FParentIndex]->FChild2;
    }
    else
    {
        sibling = FNodes[Node->FParentIndex]->FChild1;
    }

    if (grandParent != -1u)
    {
        // Destroy parent and connect sibling to grandParent.
        if (FNodes[grandParent]->FChild1 == parent)
        {
            FNodes[grandParent]->FChild1 = sibling;
        }
        else
        {
            FNodes[grandParent]->FChild2 = sibling;
        }
        FNodes[sibling]->FParentIndex = grandParent;
        FreeNode(parent);

        // Adjust ancestor bounds.
        u32 index = grandParent;
        while (index != -1u)
        {
            index = BalanceNode(index);

            u32 child1 = FNodes[grandParent]->FChild1;
            u32 child2 = FNodes[grandParent]->FChild2;

            FNodes[index]->FAABB = AABB3f::Union(FNodes[child1]->FAABB, FNodes[child2]->FAABB);
            FNodes[index]->FHeight = 1 + Max(FNodes[child1]->FHeight, FNodes[child2]->FHeight);

            index = FNodes[index]->FParentIndex;
        }
    }
    else
    {
        FRootIndex = sibling;
        FNodes[sibling]->FParentIndex = -1u;
        FreeNode(parent);
    }

    Node->FParentIndex = -1u;
}

u32 AABBTree::AllocateNewLeaf(const RigidBody* Body)
{
    u32 newLeafIndex = AllocateNewNode();
    AABBNode*& newNode = FNodes[newLeafIndex];

    mat4 BodyTransform = ComputeBodyTransform(Body);
    newNode->FBodyHandle = Body->FHandle;
    newNode->FAABB = ComputeAABB(Body, BodyTransform);

    const float fatteningValue = Physics::GetConfig().FAABBFattenValue;
    newNode->FAABB.Inflate(fatteningValue);

    newNode->FHeight = 0;
    newNode->FbIsLeaf = true;

    AlwaysCheckedAssert(FHandleToNodeMap.find(Body->FHandle) == FHandleToNodeMap.end());
    FHandleToNodeMap[Body->FHandle] = newLeafIndex;

    return newLeafIndex;
}

u32 AABBTree::AllocateNewNode()
{
    u32 nodeIndex = -1u;
    if (FFreeNodeIndices.empty())
    {
        nodeIndex = FNodes.size();
        AABBNode*& newNode = FNodes.emplace_back(new AABBNode);
    }
    else
    {
        nodeIndex = *FFreeNodeIndices.begin();
        FFreeNodeIndices.erase(FFreeNodeIndices.begin());
        AssertRelease(nodeIndex < FNodes.size());
        AssertRelease(FNodes[nodeIndex] != nullptr);
    }

    return nodeIndex;
}

void AABBTree::FreeNode(const u32 Index)
{
    FNodes[Index]->Reset();
    FFreeNodeIndices.insert(Index);
}

u32 AABBTree::FindBestSibling(const u32 NewLeafIndex) const
{
    AssertRelease(NewLeafIndex < FNodes.size());
    AssertRelease(NewLeafIndex != FRootIndex);
    AssertRelease(FNodes.size() > 1);
    const AABBNode* NodeToAdd = FNodes[NewLeafIndex];
    AssertRelease(NodeToAdd != nullptr);
    const float NewNodeSA = SurfaceArea(NodeToAdd->FAABB);

    // init variables
    u32 bestNode = FRootIndex;
    while (!FNodes[bestNode]->FbIsLeaf)
    {
        const AABBNode* NodeToCheck = FNodes[bestNode];
        AssertRelease(NodeToCheck != nullptr);

        // Compute cost for current node
        AABB3f newAABB = AABB3f::Union(NodeToAdd->FAABB, NodeToCheck->FAABB);
        float currentCostForAddingLeafHere = SurfaceArea(newAABB);
        float currentNodeCost = SurfaceArea(NodeToCheck->FAABB);

        float inheritanceCost = currentCostForAddingLeafHere - currentNodeCost;

        // check the children
        u32 FChild1 = NodeToCheck->FChild1;
        u32 FChild2 = NodeToCheck->FChild2;

        // check the first one
        float cost1 = 0.f;
        newAABB = AABB3f::Union(NodeToAdd->FAABB, FNodes[FChild1]->FAABB);
        float newArea = SurfaceArea(newAABB);
        if (FNodes[FChild1]->FbIsLeaf)
        {
            cost1 = newArea + inheritanceCost;
        }
        else
        {
            float oldArea = SurfaceArea(FNodes[FChild1]->FAABB);
            cost1 = (newArea - oldArea) + inheritanceCost;
        }

        // check the second one
        float cost2 = 0.f;
        newAABB = AABB3f::Union(NodeToAdd->FAABB, FNodes[FChild2]->FAABB);
        newArea = SurfaceArea(newAABB);
        if (FNodes[FChild2]->FbIsLeaf)
        {
            cost2 = newArea + inheritanceCost;
        }
        else
        {
            float oldArea = SurfaceArea(FNodes[FChild2]->FAABB);
            cost2 = (newArea - oldArea) + inheritanceCost;
        }

        if (currentNodeCost < cost1 && currentNodeCost < cost2)
        {
            break;
        }

        if (cost1 < cost2)
        {
            bestNode = FChild1;
        }
        else
        {
            bestNode = FChild2;
        }
    }

    return bestNode;
}

u32 AABBTree::BalanceNode(const u32 iA)
{
    AABBNode* A = FNodes[iA];
    if (A->FbIsLeaf || A->FHeight < 2)
    {
        return iA;
    }

    i32 iB = A->FChild1;
    i32 iC = A->FChild2;
    AssertRelease(0 <= iB && iB < FNodes.size());
    AssertRelease(0 <= iC && iC < FNodes.size());

    AABBNode* B = FNodes[iB];
    AABBNode* C = FNodes[iC];

    i32 balance = C->FHeight - B->FHeight;

    // Rotate C up
    if (balance > 1)
    {
        i32 iF = C->FChild1;
        i32 iG = C->FChild2;
        AABBNode* F = FNodes[iF];
        AABBNode* G = FNodes[iG];
        AssertRelease(0 <= iF && iF < FNodes.size());
        AssertRelease(0 <= iG && iG < FNodes.size());

        // Swap A and C
        C->FChild1 = iA;
        C->FParentIndex = A->FParentIndex;
        A->FParentIndex = iC;

        // A's old FParentIndex should point to C
        if (C->FParentIndex != -1u)
        {
            if (FNodes[C->FParentIndex]->FChild1 == iA)
            {
                FNodes[C->FParentIndex]->FChild1 = iC;
            }
            else
            {
                AssertRelease(FNodes[C->FParentIndex]->FChild2 == iA);
                FNodes[C->FParentIndex]->FChild2 = iC;
            }
        }
        else
        {
            FRootIndex = iC;
        }

        // Rotate
        if (F->FHeight > G->FHeight)
        {
            C->FChild2 = iF;
            A->FChild2 = iG;
            G->FParentIndex = iA;
            A->FAABB = AABB3f::Union(B->FAABB, G->FAABB);
            C->FAABB = AABB3f::Union(A->FAABB, F->FAABB);

            A->FHeight = 1 + std::max(B->FHeight, G->FHeight);
            C->FHeight = 1 + std::max(A->FHeight, F->FHeight);
        }
        else
        {
            C->FChild2 = iG;
            A->FChild2 = iF;
            F->FParentIndex = iA;
            A->FAABB = AABB3f::Union(B->FAABB, F->FAABB);
            C->FAABB = AABB3f::Union(A->FAABB, G->FAABB);

            A->FHeight = 1 + std::max(B->FHeight, F->FHeight);
            C->FHeight = 1 + std::max(A->FHeight, G->FHeight);
        }

        return iC;
    }

    // Rotate B up
    if (balance < -1)
    {
        i32 iD = B->FChild1;
        i32 iE = B->FChild2;
        AABBNode* D = FNodes[iD];
        AABBNode* E = FNodes[iE];
        AssertRelease(0 <= iD && iD < FNodes.size());
        AssertRelease(0 <= iE && iE < FNodes.size());

        // Swap A and B
        B->FChild1 = iA;
        B->FParentIndex = A->FParentIndex;
        A->FParentIndex = iB;

        // A's old FParentIndex should point to B
        if (B->FParentIndex != -1u)
        {
            if (FNodes[B->FParentIndex]->FChild1 == iA)
            {
                FNodes[B->FParentIndex]->FChild1 = iB;
            }
            else
            {
                AssertRelease(FNodes[B->FParentIndex]->FChild2 == iA);
                FNodes[B->FParentIndex]->FChild2 = iB;
            }
        }
        else
        {
            FRootIndex = iB;
        }

        // Rotate
        if (D->FHeight > E->FHeight)
        {
            B->FChild2 = iD;
            A->FChild1 = iE;
            E->FParentIndex = iA;
            A->FAABB = AABB3f::Union(C->FAABB, E->FAABB);
            B->FAABB = AABB3f::Union(A->FAABB, D->FAABB);

            A->FHeight = 1 + std::max(C->FHeight, E->FHeight);
            B->FHeight = 1 + std::max(A->FHeight, D->FHeight);
        }
        else
        {
            B->FChild2 = iE;
            A->FChild1 = iD;
            D->FParentIndex = iA;
            A->FAABB = AABB3f::Union(C->FAABB, D->FAABB);
            B->FAABB = AABB3f::Union(A->FAABB, E->FAABB);

            A->FHeight = 1 + std::max(C->FHeight, D->FHeight);
            B->FHeight = 1 + std::max(A->FHeight, E->FHeight);
        }

        return iB;
    }

    return iA;
}

float AABBTree::SurfaceArea(const AABB3f& Node) const
{
    const vec3 AABBExtents = Node.Extent();
    return /*2.f **/ (AABBExtents.x * AABBExtents.y + AABBExtents.x * AABBExtents.z + AABBExtents.y * AABBExtents.z);
}

mat4 AABBTree::ComputeBodyTransform(const RigidBody* Body) const
{
    return Body->GetTransform();
}

AABB3f AABBTree::ComputeAABB(const RigidBody* Body, const mat4& Transform) const
{
    return Body->FCollisionShape.ComputeAABB(Transform);
}

AABB3f AABBTree::ComputeAABB(const RigidBody* Body) const
{
    mat4 Transform = ComputeBodyTransform(Body);
    return ComputeAABB(Body, Transform);
}

u32 AABBTree::RetrieveNodeForBody(const RigidBody* Body) const
{
    AssertRelease(Body != nullptr);
    return RetrieveNodeForHandle(Body->FHandle);
}
u32 AABBTree::RetrieveNodeForHandle(const PhysicsBodyHandle& BodyHandle) const
{
    AssertRelease(BodyHandle.IsValid());
    auto itFind = FHandleToNodeMap.find(BodyHandle);
    if (itFind == FHandleToNodeMap.end())
    {
        return -1u;
    }
    return itFind->second;
}

void AABBNode::Reset()
{
    FAABB = AABB3f();
    FBodyHandle = PhysicsBodyHandle();
    FParentIndex = -1u;
    FChild1 = -1u;
    FChild2 = -1u;
    FHeight = -1;
    FbIsLeaf = false;
    bWasMoved = false;
}

} // namespace Physics
} // namespace ECSEngine
