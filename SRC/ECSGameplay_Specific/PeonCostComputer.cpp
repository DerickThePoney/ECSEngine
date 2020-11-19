#include "stdafx.h"

#include "PeonCostComputer.h"

namespace ECSEngine
{
u32 PeonCostComputer::GetCostForNextSpawn(const u32 parCurrentPeonsQuantity) const
{
    return (u32)floor((float)FBaseCost * pow(FMultiplier, (float)parCurrentPeonsQuantity));
}
} // namespace ECSEngine