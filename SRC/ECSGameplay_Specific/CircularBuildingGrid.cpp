#include "stdafx.h"

#include "CircularBuildingGrid.h"

#include "Common/AngleRange.h"
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

void CircularBuildingGrid::DrawFeedback() const
{
    const u32 color = ColorUtils::ConvertToU32(GameplayConstants::CircularBuildingGrid::GridFeedbackColor);
    foreachitemconst(chunk, FChunks)
    {
        const float chunkRadius = GetRadiusForChunk(chunk.Index);
        const float innerRadius = chunkRadius - 0.5f * GameplayConstants::CircularBuildingGrid::GridChunkWidth;
        const float outerRadius = chunkRadius + 0.5f * GameplayConstants::CircularBuildingGrid::GridChunkWidth;
        GameplayFeedbackDrawer::Instance().AddGridChunk(innerRadius, outerRadius, GameplayConstants::CircularBuildingGrid::GridChunkFeedbackThickness, chunk.ActualArcAngle, color);
    }
}

CircularGridAccessor CircularBuildingGrid::GetAccessorForWorldPosition(const glm::vec3& parWorldPosition) const
{
    glm::vec2 worldPos2D = glm::xz(parWorldPosition);
    const float distanceToCenter = glm::length(worldPos2D);

    // detect chunk
    foreachitemconst(chunk, FChunks)
    {
        const float radius = GetRadiusForChunk(chunk.Index);
        const float minR = radius - 0.5f * GameplayConstants::CircularBuildingGrid::GridChunkWidth;
        const float maxR = radius + 0.5f * GameplayConstants ::CircularBuildingGrid::GridChunkWidth;

        // we are less than the current chunk, abort
        if (distanceToCenter < minR)
            break;

        // we are further than the current chunk, continue
        if (distanceToCenter > maxR)
            continue;

        // We are in this circular chunk
        // now actually get the input angle
        float inputAngle = glm::atan(worldPos2D.y, worldPos2D.x);
        if (inputAngle < 0.f)
            inputAngle += 2.f * glm::pi<float>();

        foreachitemconst(cell, chunk.GridCells)
        {
            AngleRange cellAngleRange(GetCellAngleRange(cell.Index, chunk.ActualArcAngle));

            if (!cellAngleRange.Contains(inputAngle))
                continue;

            const float centerAngle = GetCellCenterAngle(cell.Index, chunk.ActualArcAngle);
            const glm::vec3 cellPosition = glm::vec3(radius * glm::cos(centerAngle), parWorldPosition.y, radius * glm::sin(centerAngle));
            return CircularGridAccessor(cellPosition, chunk.Index, cell.Index);
        }
        AssertNotReached();
    }

    return CircularGridAccessor();
}

bool CircularBuildingGrid::IsPositionFree(const u32 parChunkId, const u32 parCellId) const
{
    AssertRelease(parChunkId < FChunks.size());
    const CircularGridChunk& chunk = FChunks[parChunkId];
    AssertRelease(parCellId < chunk.CellNumber);
    return !chunk.GridCells[parCellId].Occupied;
}

void CircularBuildingGrid::SetPositionOccupied(const u32 parChunkId, const u32 parCellId, const bool parOccupied)
{
    AssertRelease(parChunkId < FChunks.size());
    CircularGridChunk& chunk = FChunks[parChunkId];
    AssertRelease(parCellId < chunk.CellNumber);
    AlwaysCheckedAssert(chunk.GridCells[parCellId].Occupied != parOccupied);
    chunk.GridCells[parCellId].Occupied = parOccupied;
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
    chunk.ActualArcAngle = 2.0f * glm::pi<float>() / chunk.CellNumber;

    AssertRelease(chunk.CellNumber != 0);
    AssertRelease(chunk.ActualArcAngle != 0);

    chunk.Index = gridChunkIndex;

    chunk.GridCells.resize(chunk.CellNumber);

    forrange(i, 0, chunk.CellNumber)
    {
        CircularGridCell& cell = chunk.GridCells[i];
        cell.Index = (u32)i;
    }
}

float CircularBuildingGrid::GetRadiusForChunk(const u32 parChunkIndex) const
{
    return GameplayConstants::CircularBuildingGrid::GridStartRadius + parChunkIndex * GameplayConstants::CircularBuildingGrid::GridChunkWidth +
          parChunkIndex * GameplayConstants::CircularBuildingGrid::InterChunkLength + 0.5f * GameplayConstants::CircularBuildingGrid::GridChunkWidth;
}

float CircularBuildingGrid::GetCellCenterAngle(const u32 parIndex, const float parCellAngleRange) const
{
    return parIndex * parCellAngleRange;
}

std::pair<float, float> CircularBuildingGrid::GetCellAngleRange(const u32 parIndex, float parCellAngleRange) const
{
    const float centerAngle = GetCellCenterAngle(parIndex, parCellAngleRange);
    return { centerAngle - 0.5f * parCellAngleRange, centerAngle + 0.5f * parCellAngleRange };
}

CircularGridAccessor::CircularGridAccessor()
{
}

CircularGridAccessor::CircularGridAccessor(const glm::vec3 parGridCellPosition, const u32 parChunkId, const u32 parCellId)
    : FGridCellPosition(parGridCellPosition)
    , FChunkId(parChunkId)
    , FCellId(parCellId)
    , FValid(true)
{
}

bool CircularGridAccessor::IsFree() const
{
    if (!FValid)
        return false;

    return CircularBuildingGrid::Instance().IsPositionFree(FChunkId, FCellId);
}

void CircularGridAccessor::SetOccupied(bool parOccupied) const
{
    AssertRelease(FValid);
    CircularBuildingGrid::Instance().SetPositionOccupied(FChunkId, FCellId, parOccupied);
}

} // namespace ECSEngine
