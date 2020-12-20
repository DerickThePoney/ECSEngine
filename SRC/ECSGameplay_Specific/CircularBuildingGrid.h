#pragma once
#include "Common/Singleton.h"

namespace ECSEngine
{

class CircularBuildingGrid : public Singleton<CircularBuildingGrid>
{
public:
    void Initialise();
    void Shutdown();

    void DrawFeedback();

private:
    void CreateNewGridChunk();
    float GetRadiusForChunk(const u32 parChunkIndex) const;

private:
    struct CircularGridCell
    {
        u32 index = 0;
        bool occupied = false;
    };
    struct CircularGridChunk
    {
        ~CircularGridChunk() { delete[] GridCells; }

        CircularGridCell* GridCells = nullptr;
        float ActualArcLength = 0;
        u32 CellNumber = 0;
        u32 Index = 0;
    };

    std::vector<CircularGridChunk> FChunks;
};
} // namespace ECSEngine