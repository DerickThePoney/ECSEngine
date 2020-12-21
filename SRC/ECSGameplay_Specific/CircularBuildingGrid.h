#pragma once
#include "Common/Singleton.h"

namespace ECSEngine
{
class CircularGridAccessor
{
public:
    CircularGridAccessor(const glm::vec3 parGridCellPosition = glm::vec3(0.f), const bool parValid = false);

    bool Valid() const { return FValid; }
    glm::vec3 CellPosition() const { return FGridCellPosition; }

private:
    glm::vec3 FGridCellPosition = glm::vec3(0.f);
    bool FValid = false;
};

class CircularBuildingGrid : public Singleton<CircularBuildingGrid>
{
public:
    void Initialise();
    void Shutdown();

    void DrawFeedback() const;

    CircularGridAccessor GetAccessorForWorldPosition(const glm::vec3& parWorldPosition) const;

private:
    void CreateNewGridChunk();
    float GetRadiusForChunk(const u32 parChunkIndex) const;
    float GetCellCenterAngle(const u32 parIndex, const float parCellAngleRange) const;
    std::pair<float, float> GetCellAngleRange(const u32 parIndex, const float parCellAngleRange) const;

private:
    struct CircularGridCell
    {
        u32 index = 0;
        bool occupied = false;
    };
    struct CircularGridChunk
    {
        std::vector<CircularGridCell> GridCells;
        float ActualArcAngle = 0;
        u32 CellNumber = 0;
        u32 Index = 0;
    };

    std::vector<CircularGridChunk> FChunks;
};
} // namespace ECSEngine