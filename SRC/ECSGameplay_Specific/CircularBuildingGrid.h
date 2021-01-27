#pragma once
#include "CircularPathfindingGraph.h"
#include "Common/Singleton.h"

namespace ECSEngine
{
class CircularGridAccessor
{
public:
    CircularGridAccessor();
    CircularGridAccessor(const glm::vec3 parGridCellPosition, const u32 parChunkId, const u32 parCellId);

    bool Valid() const { return FValid; }
    glm::vec3 CellPosition() const { return FGridCellPosition; }
    bool IsFree(const u32 parBuildingSize) const;

    void SetOccupied(bool parOccupied, const u32 parBuildingSize) const;

    u32 ChunkId() const { return FChunkId; }
    u32 CellId() const { return FCellId; }

private:
    glm::vec3 FGridCellPosition = glm::vec3(0.f);
    u32 FChunkId = -1;
    u32 FCellId = -1;
    bool FValid = false;
};

class CircularBuildingGrid : public Singleton<CircularBuildingGrid>
{
public:
    void Initialise();
    void Shutdown();

    void DrawFeedback() const;

    CircularGridAccessor GetAccessorForWorldPosition(const glm::vec3& parWorldPosition) const;
    bool IsPositionFree(const u32 parChunkId, const u32 parCellId) const;

    void SetPositionOccupied(const u32 parChunkId, const u32 parCellId, const bool parOccupied);

    u32 GetCellCount(const u32 parChunkId) const;
    const Pathfinding::CircularPathfindingGraph& GetGraph() const { return FGraph; }

private:
    void CreateNewGridChunk();
    float GetRadiusForChunk(const u32 parChunkIndex) const;
    float GetCellCenterAngle(const u32 parIndex, const float parCellAngleRange) const;
    std::pair<float, float> GetCellAngleRange(const u32 parIndex, const float parCellAngleRange) const;

private:
    struct CircularGridCell
    {
        u32 Index = 0;
        bool Occupied = false;
    };
    struct CircularGridChunk
    {
        std::vector<CircularGridCell> GridCells;
        float ActualArcAngle = 0;
        u32 CellCount = 0;
        u32 Index = 0;
    };

    std::vector<CircularGridChunk> FChunks;

    Pathfinding::CircularPathfindingGraph FGraph;
};
} // namespace ECSEngine