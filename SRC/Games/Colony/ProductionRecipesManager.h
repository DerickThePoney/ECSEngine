#pragma once
#include "Common/MemoryView.h"
#include "ProductionRecipe.h"

namespace ECSEngine
{
class ProductionRecipesManager
{
public:
    SERIALIZE() { PROPERTYFIELD(Recipes, std::vector<ProductionRecipe>()); }

    void DrawInEditor();

    MemoryView<const ProductionRecipe> ProductionRecipes() const { return MemoryView(FRecipes.data(), FRecipes.size()); }
    const ProductionRecipe* GetProductionRecipe(const std::string& parName) const;

private:
    std::vector<ProductionRecipe> FRecipes;
};
} // namespace ECSEngine