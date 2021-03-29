#pragma once
#include "GameResources.h"

namespace ECSEngine
{
class PeonCostComputer
{
public:
    u32 GetCostForNextSpawn(const u32 parCurrentPeonsQuantity) const;

    SERIALIZE()
    {
        PROPERTYFIELD(BaseCost, 10);
        PROPERTYFIELD(Multiplier, 1.05f);
        PROPERTYFIELD(ResourceToPay, GameResource::FOOD);
    }

    static void DrawEditingHeader();
    void DrawEditor();

    u32 FBaseCost = 10;
    float FMultiplier = 1.05f;
    GameResource::Type FResourceToPay = GameResource::FOOD;
};
} // namespace ECSEngine
