#include "stdafx.h"

#include "CircularBuildingGrid.h"

#include "CircularGridAccessor.h"
#include "Common/AngleRange.h"
#include "Common/ColorUtils.h"
#include "Common/Polygon.h"
#include "Common/SavingSystemImplementation.h"
#include "ECSCore/AdjustableDebugParameters.h"
#include "GameplayConstants.h"
#include "GameplayFeedbackDrawer.h"

namespace ECSEngine
{
IMPLEMENT_SAVELOAD_ABILITIES(CircularBuildingGrid);
template<typename Chunk, bool isWriting>
void CircularBuildingGrid::SaveLoad(Chunk& parChunk)
{
    u32 nbChunks = FChunks.size();
    parChunk& nbChunks;
    if (!isWriting)
    {
        if (nbChunks != FChunks.size())
        {
            FChunks.clear();
            FGraph.Cleanup();
            forrange(i, 0, nbChunks) { CreateNewGridChunk(); }
        }
    }
}

void CircularBuildingGrid::Initialise()
{
    forrange(i, 0, GameplayConstants::CircularBuildingGrid::StartingGridChunkNumber) { CreateNewGridChunk(); }
}

void CircularBuildingGrid::Shutdown()
{
    FChunks.clear();
    FGraph.Cleanup();
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

CircularGridAccessor CircularBuildingGrid::GetAccessorForWorldPosition(const vec3& parWorldPosition) const
{
    const vec2 worldPos2D = parWorldPosition.xz();
    const float distanceToCenter = Length(worldPos2D);

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
        float inputAngle = atan(worldPos2D.y, worldPos2D.x);
        if (inputAngle < 0.f)
            inputAngle += 2.f * Pi();

        foreachitemconst(cell, chunk.GridCells)
        {
            AngleRange cellAngleRange(GetCellAngleRange(cell.Index, chunk.ActualArcAngle));

            if (!cellAngleRange.Contains(inputAngle))
                continue;

            const float centerAngle = GetCellCenterAngle(cell.Index, chunk.ActualArcAngle);
            const vec3 cellPosition = vec3(radius * cos(centerAngle), parWorldPosition.y, radius * sin(centerAngle));
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
    AssertRelease(parCellId < chunk.CellCount);
    return !chunk.GridCells[parCellId].Occupied;
}

void CircularBuildingGrid::SetPositionOccupied(const u32 parChunkId, const u32 parCellId, const bool parOccupied)
{
    AssertRelease(parChunkId < FChunks.size());
    CircularGridChunk& chunk = FChunks[parChunkId];
    AssertRelease(parCellId < chunk.CellCount);
    AlwaysCheckedAssert(chunk.GridCells[parCellId].Occupied != parOccupied);
    chunk.GridCells[parCellId].Occupied = parOccupied;
}

u32 CircularBuildingGrid::GetCellCount(const u32 parChunkId) const
{
    AssertRelease(parChunkId < FChunks.size());
    return FChunks[parChunkId].CellCount;
}

Polygon2D CircularBuildingGrid::CreatePolygon(const CircularGridAccessor& parGridAccessor, u32 parBuildingSize) const
{
    /// PROBLEMS WITH MULTISIZED BUILDINGS !

    const u32 cellCount = GetCellCount(parGridAccessor.ChunkId());
    i32 startIdx = (parBuildingSize > 1) ? -(i32)ceil(parBuildingSize / 2.f - 1.f) : 0;
    i32 endIdx = startIdx + (parBuildingSize - 1);

    auto convertIdxToCellId = [](const CircularGridAccessor& parGridAccessor, const i32 cellIdx, const u32 cellCount) -> i32
    {
        i32 cellId = parGridAccessor.CellId() + cellIdx;
        if (cellId < 0)
            cellId += cellCount;
        AlwaysCheckedAssert(cellId >= 0);
        cellId = cellId % cellCount;
        return cellId;
    };
    startIdx = convertIdxToCellId(parGridAccessor, startIdx, cellCount);
    endIdx = convertIdxToCellId(parGridAccessor, endIdx, cellCount);

    Polygon2D res;

    // find inner limits
    const float radius = GetRadiusForChunk(parGridAccessor.ChunkId());
    const float minR = radius - 0.4f * GameplayConstants::CircularBuildingGrid::GridChunkWidth;
    const float maxR = radius + 0.4f * GameplayConstants ::CircularBuildingGrid::GridChunkWidth;
    AngleRange startCellAngleRange(GetCellAngleRange(startIdx, FChunks[parGridAccessor.ChunkId()].ActualArcAngle));
    AngleRange endCellAngleRange(GetCellAngleRange(endIdx, FChunks[parGridAccessor.ChunkId()].ActualArcAngle));

    ADJUSTABLE_DEBUG_PARAMETER_SINGLE(freeColisionSpace, 0.1f, "Free colision space", "CircularBuildingGrid/Obstacles", 0.f, 0.5f);
    const float freeSpaceAngle = freeColisionSpace / radius;
    const float angleDiff = AngleHelpers::AngleDifference(startCellAngleRange.Left(), endCellAngleRange.Right()) - 2.f * freeSpaceAngle;
    //-2.0f * radians(1.5f);
    AlwaysCheckedAssert(angleDiff > 0.f);
    const u32 wantedSubdiv = (u32)(angleDiff / Radians(5.f));
    const float angleInterv = angleDiff / (wantedSubdiv - 1);

    // fill the polygon
    res.reserve((wantedSubdiv + 1) * 2);
    forrange(i, 0, wantedSubdiv)
    {
        const float angle = startCellAngleRange.Left() + freeSpaceAngle + i * angleInterv;
        const vec2 pos = minR * vec2(cos(angle), sin(angle));
        res.push_back(pos);
    }

    reverseforrange(i, 0, wantedSubdiv)
    {
        const float angle = startCellAngleRange.Left() + freeSpaceAngle + i * angleInterv;
        const vec2 pos = maxR * vec2(cos(angle), sin(angle));
        res.push_back(pos);
    }
    return res;
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
    const float nbSubdiv = 2.0f * Pi() / wantedAngleSubdiv;

    FChunks.push_back(CircularGridChunk());
    CircularGridChunk& chunk = FChunks[gridChunkIndex];

    // round the cell number and compute the actual arclength using this number
    chunk.CellCount = (u32)std::roundf(nbSubdiv);
    chunk.ActualArcAngle = 2.0f * Pi() / chunk.CellCount;

    AssertRelease(chunk.CellCount != 0);
    AssertRelease(chunk.ActualArcAngle != 0);

    chunk.Index = gridChunkIndex;

    chunk.GridCells.resize(chunk.CellCount);

    forrange(i, 0, chunk.CellCount)
    {
        CircularGridCell& cell = chunk.GridCells[i];
        cell.Index = (u32)i;
    }
    FGraph.AddNewCircularChunk();
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

} // namespace ECSEngine
