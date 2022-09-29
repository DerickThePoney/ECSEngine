#include "stdafx.h"

#include "CircularGridAccessor.h"

#include "CircularBuildingGrid.h"
#include "Common/Polygon.h"
#include "Common/SavingSystemImplementation.h"

namespace ECSEngine
{
IMPLEMENT_SAVELOAD_ABILITIES(CircularGridAccessor);
template<typename Chunk, bool isWriting>
void CircularGridAccessor::SaveLoad(Chunk& parChunk)
{
    parChunk& FGridCellPosition;
    parChunk& FChunkId;
    parChunk& FCellId;
    parChunk& FValid;
}

CircularGridAccessor::CircularGridAccessor()
{
}

CircularGridAccessor::CircularGridAccessor(const vec3 parGridCellPosition, const u32 parChunkId, const u32 parCellId)
    : FGridCellPosition(parGridCellPosition)
    , FChunkId(parChunkId)
    , FCellId(parCellId)
    , FValid(true)
{
}

bool CircularGridAccessor::IsFree(const u32 parBuildingSize) const
{
    if (!FValid)
        return false;

    const i32 startIdx = (parBuildingSize > 1) ? -(i32)ceil(parBuildingSize / 2.f - 1.f) : 0;
    const i32 endIdx = startIdx + parBuildingSize;
    const u32 cellCount = CircularBuildingGrid::Instance().GetCellCount(FChunkId);

    for (i32 idx = startIdx; idx < endIdx; ++idx)
    {
        i32 cellId = (FCellId + idx);
        if (cellId < 0)
            cellId += cellCount;
        AlwaysCheckedAssert(cellId >= 0);
        cellId = cellId % cellCount;

        if (!CircularBuildingGrid::Instance().IsPositionFree(FChunkId, cellId))
            return false;
    }

    return true;
}

void CircularGridAccessor::SetOccupied(bool parOccupied, const u32 parBuildingSize) const
{
    AssertRelease(FValid);
    const i32 startIdx = (parBuildingSize > 1) ? -(i32)ceil(parBuildingSize / 2.f - 1.f) : 0;
    const i32 endIdx = startIdx + parBuildingSize;
    const u32 cellCount = CircularBuildingGrid::Instance().GetCellCount(FChunkId);

    for (i32 idx = startIdx; idx < endIdx; ++idx)
    {
        i32 cellId = (FCellId + idx);
        if (cellId < 0)
            cellId += cellCount;
        AlwaysCheckedAssert(cellId >= 0);
        cellId = cellId % cellCount;

        CircularBuildingGrid::Instance().SetPositionOccupied(FChunkId, cellId, parOccupied);
    }
}

Polygon2D CircularGridAccessor::CreatePolygon(u32 parBuildingSize) const
{
    return CircularBuildingGrid::Instance().CreatePolygon(*this, parBuildingSize);
}

} // namespace ECSEngine
