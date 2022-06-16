#pragma once

namespace ECSEngine
{
class ProductionRecipe;
namespace ProductionRecipeHelpers
{
float ComputeCraftDuration(const ProductionRecipe* parRecipe);
float GetEfficiencyDeltaTime();
} // namespace ProductionRecipeHelpers
} // namespace ECSEngine