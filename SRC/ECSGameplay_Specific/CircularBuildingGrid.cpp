#include "stdafx.h"

#include "CircularBuildingGrid.h"

#include "Common/ColorUtils.h"
#include "GameplayConstants.h"
#include "GameplayFeedbackDrawer.h"

namespace ECSEngine
{

void CircularBuildingGrid::Initialise()
{
    forrange(i, 0, GameplayConstants::CircularBuildingGrid::StartingGridChunkNumber) { CreateNewGridChunk(); }
}

void CircularBuildingGrid::Shutdown()
{
    FChunks.clear();
}

void CircularBuildingGrid::DrawFeedback()
{
    const u32 color = ColorUtils::ConvertToU32(GameplayConstants::CircularBuildingGrid::GridFeedbackColor);
    foreachitem(chunk, FChunks)
    {
        const float chunkRadius = GetRadiusForChunk(chunk.Index);
        const float innerRadius = chunkRadius - 0.5f * GameplayConstants::CircularBuildingGrid::GridChunkWidth;
        const float outerRadius = chunkRadius + 0.5f * GameplayConstants::CircularBuildingGrid::GridChunkWidth;
        GameplayFeedbackDrawer::Instance().AddGridChunk(innerRadius, outerRadius, GameplayConstants::CircularBuildingGrid::GridChunkFeedbackThickness, color);
    }
}

void CircularBuildingGrid::CreateNewGridChunk()
{
    // wanted arc length
    const float wantedArcLength = GameplayConstants::CircularBuildingGrid::WantedArcLength;

    // get chunk radius
    const u32 gridChunkIndex = (u32)FChunks.size();
    const float gridChunkRadius = GetRadiusForChunk(gridChunkIndex);

    // compute subdivs using wanted angle
    const float wantedAngleSubdiv = wantedArcLength / gridChunkRadius;
    const float nbSubdiv = 2.0f * glm::pi<float>() / wantedAngleSubdiv;

    FChunks.push_back(CircularGridChunk());
    CircularGridChunk& chunk = FChunks[gridChunkIndex];

    // round the cell number and compute the actual arclength using this number
    chunk.CellNumber = (u32)std::roundf(nbSubdiv);
    chunk.ActualArcLength = 2.0f * glm::pi<float>() / chunk.CellNumber;

    AssertRelease(chunk.CellNumber != 0);
    AssertRelease(chunk.ActualArcLength != 0);

    chunk.Index = gridChunkIndex;

    chunk.GridCells.resize(chunk.CellNumber);

    forrange(i, 0, chunk.CellNumber)
    {
        CircularGridCell& cell = chunk.GridCells[i];
        cell.index = (u32)i;
    }
}

float CircularBuildingGrid::GetRadiusForChunk(const u32 parChunkIndex) const
{
    return GameplayConstants::CircularBuildingGrid::GridStartRadius + parChunkIndex * GameplayConstants::CircularBuildingGrid::GridChunkWidth +
          parChunkIndex * GameplayConstants::CircularBuildingGrid::InterChunkLength + 0.5f * GameplayConstants::CircularBuildingGrid::GridChunkWidth;
}

} // namespace ECSEngine
