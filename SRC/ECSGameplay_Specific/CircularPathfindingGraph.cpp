#include "stdafx.h"

#include "CircularPathfindingGraph.h"

#include "Common/CameraHelpers.h"
#include "Common/CameraManager.h"
#include "Common/InputManager.h"
#include "ECSCore/AdjustableDebugParameters.h"
#include "GameplayConstants.h"
#include "GameplayFeedbackDrawer.h"
#include "RenderingCore/DrawCommands.h"
#include "RenderingCore/GLFWDisplayWindowHandler.h"

namespace ECSEngine
{
namespace Pathfinding
{
/*
 * Pour chaque chunk circulaire, on ajoute un inner navigation ring, qui circule à l'intérieur du chunk. Cela veut dire qu'on ajoute un edge vers l'intérieur à chaque fois. On
 * ajoute ensuite un noeud correspondant pour chaque noeud de l'inner navigation ring à l'intérieur du chunk, qui permet d'aller vers l'intérieur du chunk. Ces noeuds seront
 * connectés aux noeuds de l'inner navigation ring du ring suivant pour permettre de passer au noeud suivant.
 *
 * EDIT: DANS UN PREMIER TEMPS, UNE SEULE CONNECTION ENTRE L'INNER ET L'OUTER RING
 *
 * TODO:
 * - méthode efficiente pour touver le noeud le plus proche à partir de la position. -- Une méthode trouvée
 * - méthode efficiente pour linker les chunks
 * - path solving
 */
void CircularPathfindingGraph::AddNewCircularChunk()
{
    const u32 currentChunkId = (u32)FNavigationNodes.size();

    const float chunkRadius = GameplayConstants::CircularBuildingGrid::GridStartRadius + currentChunkId * GameplayConstants::CircularBuildingGrid::GridChunkWidth +
          currentChunkId * GameplayConstants::CircularBuildingGrid::InterChunkLength + 0.5f * GameplayConstants::CircularBuildingGrid::GridChunkWidth;
    const float minR = chunkRadius - 0.5f * GameplayConstants::CircularBuildingGrid::GridChunkWidth;
    const float maxR = chunkRadius + 0.5f * GameplayConstants ::CircularBuildingGrid::GridChunkWidth;

    const float pathDistance = minR - 0.5f * GameplayConstants::CircularBuildingGrid::InterChunkLength;

    const float wantedAngleSubdiv = GameplayConstants::CircularBuildingGrid::NavigationNodesDistance / pathDistance;
    const float nbSubdiv = 2.0f * glm::pi<float>() / wantedAngleSubdiv;

    const u32 nodesCount = (u32)std::roundf(nbSubdiv);
    const float nodesAngle = 2.0f * glm::pi<float>() / nodesCount;

    std::vector<Node> newChunk;
    newChunk.reserve(nodesCount);

    std::vector<Edges> newEdges;
    newEdges.reserve(nodesCount + 1 + ((currentChunkId == 0) ? 0 : 1));

    forrange(i, 0, nodesCount)
    {
        const float angle = i * nodesAngle;
        glm::vec2 currentPos(pathDistance * glm::cos(angle), pathDistance * glm::sin(angle));
        newChunk.push_back(Node{ currentPos });
        const u32 edgeId = (u32)newEdges.size();
        const u32 prevNode = ((i == 0) ? nodesCount - 1 : (u32)i - 1);
        const u32 nextNode = ((u32)i + 1) % nodesCount;

        newEdges.push_back({ { currentChunkId, prevNode }, { currentChunkId, (u32)i } });
        newEdges.push_back({ { currentChunkId, (u32)i }, { currentChunkId, nextNode } });

        FAdjacencyMap[{ currentChunkId, prevNode }].push_back(edgeId);
        FAdjacencyMap[{ currentChunkId, (u32)i }].push_back(edgeId + 1);
    }

    if (currentChunkId > 0)
    {
        const u32 edgeId = (u32)newEdges.size();
        newEdges.push_back({ { currentChunkId - 1, 0 }, { currentChunkId, 0 } });
        newEdges.push_back({ { currentChunkId, 0 }, { currentChunkId - 1, 0 } });
        FAdjacencyMap[{ currentChunkId - 1, 0 }].push_back(edgeId);
        FAdjacencyMap[{ currentChunkId, 0 }].push_back(edgeId + 1);
    }

    FEdges.reserve(FEdges.size() + newEdges.size());
    FEdges.insert(FEdges.end(), newEdges.begin(), newEdges.end());
    FNavigationNodes.push_back(newChunk);
}

void CircularPathfindingGraph::Cleanup()
{
    FEdges.clear();
    FNavigationNodes.clear();
}

void CircularPathfindingGraph::Debug(Rendering::DrawCommandBuffer& parBuffer, const Rendering::MaterialInstanceHandle& parMaterial) const
{
    std::vector<glm::vec2> lines;
    lines.reserve(FEdges.size() + 1);
    lines.push_back(glm::vec2(0.f));
    foreachitemconst(edge, FEdges)
    {
        const glm::vec2 start = FNavigationNodes[edge.FStart.ChunkId][edge.FStart.Id].FPosition;
        lines.push_back(start);
    }

    parBuffer.DrawLines(parMaterial, lines, (u32)lines.size(), 0.f, 0xFFFF00FF, false);

    ADJUSTABLE_DEBUG_PARAMETER_BOOLEAN(showPositionClosestToMouse, false, "Show position closest to mouse", "Pathfinding/CircularGraph");
    if (!showPositionClosestToMouse)
        return;

    u32 FCameraId = CameraManager::Instance().CreateCameraIFN("GameplayCamera");
    Camera* camera = CameraManager::Instance().GetCamera(FCameraId);
    const glm::uvec2 windowSize = Rendering::GLFWDisplayWindowHandler::Instance().GetSize();
    const float aspectRatio = Rendering::GLFWDisplayWindowHandler::Instance().AspectRatio();

    bool foundPos = false;
    glm::vec3 mouseWorldPosition = GetWorldPositionFromScreenPosition(*camera, aspectRatio, windowSize, Input::GetMousePosition(), foundPos);

    NodeId closestNode = GetClosestNode(mouseWorldPosition);
    if (closestNode.ChunkId == -1)
        return;

    const glm::vec3 pos = glm::vec3(FNavigationNodes[closestNode.ChunkId][closestNode.Id].FPosition.x, 0, FNavigationNodes[closestNode.ChunkId][closestNode.Id].FPosition.y);
    GameplayFeedbackDrawer::Instance().AddAABB(pos - glm::vec3(0.1f), pos + glm::vec3(0.1f), 0xFF00FFFF, glm::identity<glm::mat4>(), true);
}

CircularPathfindingGraph::NodeId CircularPathfindingGraph::GetClosestNode(const glm::vec3& parWorldPosition) const
{
    const glm::vec2 worldPos2D = glm::xz(parWorldPosition);
    const float distanceToCenter = glm::length(worldPos2D);

    // detect chunk
    forrange(i, 0, FNavigationNodes.size())
    {
        const float chunkRadius = GameplayConstants::CircularBuildingGrid::GridStartRadius + i * GameplayConstants::CircularBuildingGrid::GridChunkWidth +
              i * GameplayConstants::CircularBuildingGrid::InterChunkLength + 0.5f * GameplayConstants::CircularBuildingGrid::GridChunkWidth;
        const float minR = chunkRadius - 0.5f * GameplayConstants::CircularBuildingGrid::GridChunkWidth;
        float pathDistance = minR - 0.5f * GameplayConstants::CircularBuildingGrid::InterChunkLength;

        u32 chunkId = (u32)i;
        if ((distanceToCenter > pathDistance) && (i < FNavigationNodes.size() - 1))
        {
            // check nextNode
            const float nextChunkRadius = GameplayConstants::CircularBuildingGrid::GridStartRadius + (i + 1) * GameplayConstants::CircularBuildingGrid::GridChunkWidth +
                  (i + 1) * GameplayConstants::CircularBuildingGrid::InterChunkLength + 0.5f * GameplayConstants::CircularBuildingGrid::GridChunkWidth;
            const float nextMinR = nextChunkRadius - 0.5f * GameplayConstants::CircularBuildingGrid::GridChunkWidth;
            const float nextPathDistance = nextMinR - 0.5f * GameplayConstants::CircularBuildingGrid::InterChunkLength;
            if (distanceToCenter > nextPathDistance)
            {
                continue;
            }
            else if ((nextPathDistance - distanceToCenter) < (distanceToCenter - pathDistance))
            {
                chunkId = (u32)i + 1;
                pathDistance = nextPathDistance;
            }
        }

        // We are in this circular chunk
        // now actually get the input angle
        float inputAngle = glm::atan(worldPos2D.y, worldPos2D.x);
        if (inputAngle < 0.f)
            inputAngle += 2.f * glm::pi<float>();

        const float wantedAngleSubdiv = GameplayConstants::CircularBuildingGrid::NavigationNodesDistance / pathDistance;
        const float nbSubdiv = 2.0f * glm::pi<float>() / wantedAngleSubdiv;

        const u32 nodesCount = (u32)std::roundf(nbSubdiv);
        const float nodesAngle = 2.0f * glm::pi<float>() / nodesCount;

        const float nodeIdxFlt = inputAngle / nodesAngle;
        const float interp = nodeIdxFlt - (float)((u32)nodeIdxFlt); // eurk
        if (interp > 0.5f)
            return { chunkId, ((u32)nodeIdxFlt + 1) % (FNavigationNodes[chunkId].size()) };
        else
            return { chunkId, (u32)nodeIdxFlt };
    }
    return { (u32)-1, (u32)-1 };
}

} // namespace Pathfinding
} // namespace ECSEngine
