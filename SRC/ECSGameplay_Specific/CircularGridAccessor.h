#pragma once

#include "Common/SavingSystemDeclaration.h"

namespace ECSEngine
{
class Polygon2D;
class CircularGridAccessor
{
    DECLARE_SAVELOAD_ABILITIES();

public:
    CircularGridAccessor();
    CircularGridAccessor(const vec3 parGridCellPosition, const u32 parChunkId, const u32 parCellId);

    bool Valid() const { return FValid; }
    vec3 CellPosition() const { return FGridCellPosition; }
    bool IsFree(const u32 parBuildingSize) const;

    void SetOccupied(bool parOccupied, const u32 parBuildingSize) const;

    Polygon2D CreatePolygon(u32 parBuildingSize) const;

    u32 ChunkId() const { return FChunkId; }
    u32 CellId() const { return FCellId; }

private:
    vec3 FGridCellPosition = vec3(0.f);
    u32 FChunkId = -1;
    u32 FCellId = -1;
    bool FValid = false;
};
} // namespace ECSEngine