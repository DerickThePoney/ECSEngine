#pragma once
#include "CircularPathfindingGraph.h"
#include "Common/Singleton.h"

namespace ECSEngine
{
class Polygon2D;
class CircularGridAccessor;
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

    Polygon2D CreatePolygon(const CircularGridAccessor& parGridAccessor, u32 parBuildingSize) const;

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
