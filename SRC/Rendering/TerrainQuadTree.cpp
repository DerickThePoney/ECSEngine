#include "stdafx.h"

#include "TerrainQuadTree.h"

#include "Common/Camera.h"
#include "Common/Frustum.h"
#include "Common/IntersectionRoutines.h"
#include "Common/MemoryView.h"
#include "RenderingCore/GLFWDisplayWindowHandler.h"
#include "TerrainRenderer.h"

namespace ECSEngine
{
namespace Rendering
{
namespace QuadTree
{
void FillQuadTree(MemoryView<QuadTreeNode> parPreviousLoD, MemoryView<QuadTreeNode> parCurrentLoD, u32 parPreviousLoDIdxStart, u32 parCurrentLoDIdxStart, u32 parLoDLevel)
{
    AssertRelease(parCurrentLoD.size() == parPreviousLoD.size() * 4);

    forrange(i, 0, parPreviousLoD.size())
    {
        QuadTreeNode& seedNode = parPreviousLoD[i];

        vec3 c0 = seedNode.BBox.Min();
        vec3 c2 = seedNode.BBox.Max();

        vec3 m = 0.5f * (c0 + c2) - c0;

        {
            QuadTreeNode& currentNode = parCurrentLoD[i * 4];
            currentNode.BBox.SetMin(c0);
            currentNode.BBox.SetMax(c0 + m);
            currentNode.LoDLevel = parLoDLevel;

            seedNode.ChildrenIdx[0] = parCurrentLoDIdxStart;
            currentNode.NodeIdx = parCurrentLoDIdxStart++;
        }

        {
            QuadTreeNode& currentNode = parCurrentLoD[i * 4 + 1];
            currentNode.BBox.SetMin(c0 + vec3(m.x, 0.f, 0.f));
            currentNode.BBox.SetMax(c0 + m + vec3(m.x, 0.f, 0.f));
            currentNode.LoDLevel = parLoDLevel;

            seedNode.ChildrenIdx[1] = parCurrentLoDIdxStart;
            currentNode.NodeIdx = parCurrentLoDIdxStart++;
        }

        {
            QuadTreeNode& currentNode = parCurrentLoD[i * 4 + 2];
            currentNode.BBox.SetMin(c0 + vec3(m.x, 0.f, m.z));
            currentNode.BBox.SetMax(c0 + m + vec3(m.x, 0.f, m.z));
            currentNode.LoDLevel = parLoDLevel;

            seedNode.ChildrenIdx[2] = parCurrentLoDIdxStart;
            currentNode.NodeIdx = parCurrentLoDIdxStart++;
        }

        {
            QuadTreeNode& currentNode = parCurrentLoD[i * 4 + 3];
            currentNode.BBox.SetMin(c0 + vec3(0.f, 0.f, m.z));
            currentNode.BBox.SetMax(c0 + m + vec3(0.f, 0.f, m.z));
            currentNode.LoDLevel = parLoDLevel;

            seedNode.ChildrenIdx[3] = parCurrentLoDIdxStart;
            currentNode.NodeIdx = parCurrentLoDIdxStart++;
        }
    }
}
} // namespace QuadTree

void TerrainQuadTree::Initialize(const TerrainDescriptor* parDescriptor)
{
    FDescriptor = parDescriptor;

    // Count the number of nodes
    u32 numberOfNodes = 1;
    u32 currentNbNodes = 1;

    forrange(i, 1, FDescriptor->NumberLoDLevels)
    {
        currentNbNodes = currentNbNodes * 4;
        numberOfNodes += currentNbNodes;
    }

    // Create the nodes
    u32 currentLoD = FDescriptor->NumberLoDLevels - 1;

    FNodes.resize(numberOfNodes);
    FNodes[0].BBox.SetMin(vec3(0.f));
    FNodes[0].BBox.SetMax(vec3(FDescriptor->TerrainSize, 0.f, FDescriptor->TerrainSize));
    FNodes[0].LoDLevel = currentLoD--;
    FNodes[0].NodeIdx = 0;

    u32 currentStartIdx = 0;
    u32 currentEndIdx = 1;
    u32 previousLoDNumberOfNodes = 1;
    while (currentLoD < FDescriptor->NumberLoDLevels)
    {
        MemoryView<QuadTreeNode> previousLoD(&FNodes[currentStartIdx], currentEndIdx - currentStartIdx);

        previousLoDNumberOfNodes = previousLoDNumberOfNodes * 4;
        MemoryView<QuadTreeNode> nextLod(&FNodes[currentEndIdx], currentEndIdx + previousLoDNumberOfNodes - currentEndIdx);

        QuadTree::FillQuadTree(previousLoD, nextLod, currentStartIdx, currentEndIdx, currentLoD);

        currentStartIdx = currentEndIdx;
        currentEndIdx = currentStartIdx + previousLoDNumberOfNodes;

        currentLoD--;
    }
}

void TerrainQuadTree::FillRegionsToRender(const Camera& parCamera, std::vector<QuadTreeNode>& outNodesToRender)
{
    // build a stack of nodes and push the root node index onto it
    std::queue<u32> nodesStack;
    nodesStack.push(0);

    mat4 viewWorldMatrix = parCamera.GetViewWorldMatrix();
    vec4 cameraSphereForLoDTesting = viewWorldMatrix.Column(3);

    FrustumCorners frustumCorners;
    frustumCorners.InitFromCamera(parCamera, GLFWDisplayWindowHandler::Instance().AspectRatio());

    Frustum frustum;
    frustum.InitFromCorners(frustumCorners);

    while (!nodesStack.empty())
    {
        u32 currentNodeToProcess = nodesStack.front();
        nodesStack.pop();

        const QuadTreeNode& currentNode = FNodes[currentNodeToProcess];

        // needs frustum cull
        if (!Intersection::FrustumBoundingBoxIntersect(frustum, frustumCorners, currentNode.BBox))
        {
            continue;
        }

        // last level, just push out to render
        if (currentNode.LoDLevel == 0)
        {
            outNodesToRender.push_back(currentNode);
            continue;
        }

        // check if in range LoD + 1
        cameraSphereForLoDTesting.w = FDescriptor->LoDDistances[currentNode.LoDLevel - 1];
        if (!Intersection::SphereBoundingBoxIntersect(cameraSphereForLoDTesting, currentNode.BBox))
        {
            // we aren't, stop the algorithm here.
            outNodesToRender.push_back(currentNode);
            continue;
        }

        // check children if they are in the correct LoD
        bool hasAtLeastOne = false;
        for (u32 i = 0; i < 4; ++i)
        {
            const QuadTreeNode& currentChild = FNodes[currentNode.ChildrenIdx[i]];
            if (Intersection::SphereBoundingBoxIntersect(cameraSphereForLoDTesting, currentChild.BBox))
            {
                hasAtLeastOne = true;
                break;
            }
        }

        // if there is one, add the children on the LoDToRender else, render current level
        if (!hasAtLeastOne)
        {
            outNodesToRender.push_back(currentNode);
            continue;
        }

        for (u32 i = 0; i < 4; ++i)
        {
            nodesStack.push(currentNode.ChildrenIdx[i]);
        }
    }
}

// void TerrainQuadTree::FillRegionsToRender(const Camera& parCamera, std::vector<QuadTreeNode>& outNodesToRender)
//{
//     // build a stack of nodes and push the root node index onto it
//     mat4 viewWorldMatrix = parCamera.GetViewWorldMatrix();
//     vec4 cameraSphereForLoDTesting = viewWorldMatrix.Column(3);
//
//     FrustumCorners frustumCorners;
//     frustumCorners.InitFromCamera(parCamera, GLFWDisplayWindowHandler::Instance().AspectRatio());
//
//     Frustum frustum;
//     frustum.InitFromCorners(frustumCorners);
//
//     SelectLod(0, frustum, frustumCorners, cameraSphereForLoDTesting, outNodesToRender);
// }
//
// bool TerrainQuadTree::SelectLod(const u32 parQuadTreeNodeIndex,
//       const Frustum& parFrustum,
//       const FrustumCorners& parFrustumCorners,
//       vec4& parCameraSphere,
//       std::vector<QuadTreeNode>& outNodesToRender) const
//{
//     const QuadTreeNode& currentNode = FNodes[parQuadTreeNodeIndex];
//
//     if (currentNode.LoDLevel < FDescriptor->NumberLoDLevels - 1)
//     {
//         parCameraSphere.w = FDescriptor->LoDDistances[currentNode.LoDLevel];
//         if (!Intersection::SphereBoundingBoxIntersect(parCameraSphere, currentNode.BBox))
//         {
//             // Skip nodes node not intersecting current lodrange.
//             return false;
//         }
//     }
//
//     if (!Intersection::FrustumBoundingBoxIntersect(parFrustum, parFrustumCorners, currentNode.BBox))
//     {
//         return true;
//     }
//
//     if (currentNode.LoDLevel == 0)
//     {
//         outNodesToRender.push_back(currentNode);
//         return true;
//     }
//     else
//     {
//         parCameraSphere.w = FDescriptor->LoDDistances[currentNode.LoDLevel - 1];
//         if (!Intersection::SphereBoundingBoxIntersect(parCameraSphere, currentNode.BBox))
//         {
//             // We now know this node is only covering one lodrange.
//             // Add node to draw list.
//             outNodesToRender.push_back(currentNode);
//         }
//         else
//         {
//
//             // If node is within LOD and also within range of LOD - 1
//
//             // we add children of node that only covers LOD and skip
//             // children that covers LOD - 1
//             for (u32 child : currentNode.ChildrenIdx)
//             {
//                 if (!SelectLod(child, parFrustum, parFrustumCorners, parCameraSphere, outNodesToRender))
//                 {
//                     outNodesToRender.push_back(FNodes[child]);
//                 }
//             }
//         }
//     }
//     return true;
//}

} // namespace Rendering
} // namespace ECSEngine