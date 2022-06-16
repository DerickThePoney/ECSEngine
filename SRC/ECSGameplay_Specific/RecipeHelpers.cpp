#include "stdafx.h"

#include "RecipeHelpers.h"

#include "Common/TimeManager.h"
#include "EnergySystem.h"
#include "ProductionRecipe.h"

namespace ECSEngine
{
namespace ProductionRecipeHelpers
{

float ComputeCraftDuration(const ProductionRecipe* parRecipe)
{
    const float dt = GetEfficiencyDeltaTime();

    if (dt == 0.f)
        return std::numeric_limits<float>::max();

    const float baseCraftDuration = parRecipe->CraftDuration();
    return (baseCraftDuration / dt) * TimeManager::GameplayDeltaTime();
}

float GetEfficiencyDeltaTime()
{
    const float energyEfficiency = EnergySystem::Instance().EnergyEfficiency();
    return TimeManager::GameplayDeltaTime() * energyEfficiency / 100.f;
}

} // namespace ProductionRecipeHelpers
} // namespace ECSEngine
