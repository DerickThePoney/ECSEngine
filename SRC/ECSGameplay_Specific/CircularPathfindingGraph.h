#pragma once

namespace ECSEngine
{
namespace Rendering
{
class DrawCommandBuffer;
class MaterialInstanceHandle;
} // namespace Rendering
namespace Pathfinding
{
class CircularPathfindingGraph
{
public:
    struct NodeId
    {
        u32 ChunkId;
        u32 Id;

        bool operator<(const NodeId& parOther) const { return ChunkId < parOther.ChunkId && Id < parOther.Id; }
    };

public:
    void AddNewCircularChunk();
    void Cleanup();

    void Debug(Rendering::DrawCommandBuffer& parBuffer, const Rendering::MaterialInstanceHandle& parMaterial) const;

    NodeId GetClosestNode(const glm::vec3& parWorldPosition) const;

private:
    struct Node
    {
        glm::vec2 FPosition;
    };
    struct Edges
    {
        NodeId FStart;
        NodeId FEnd;
    };

    std::vector<std::vector<Node>> FNavigationNodes;
    std::vector<Edges> FEdges;
    std::map<NodeId, std::vector<u32>> FAdjacencyMap;
};
} // namespace Pathfinding
} // namespace ECSEngine
