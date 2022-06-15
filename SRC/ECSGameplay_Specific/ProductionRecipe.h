#pragma once
#include "Common/MemoryView.h"
#include "GameResources.h"

namespace ECSEngine
{
using RecipeComponent = std::pair<GameResource::Type, u32>;

class ProductionRecipe
{
public:
    const std::string& Name() const { return FName; }
    float CraftDuration() const { return FCraftDuration; }
    MemoryView<const RecipeComponent> InputComponents() const { return MemoryView(FInputComponents.data(), FInputComponents.size()); }
    MemoryView<const RecipeComponent> OutputComponents() const { return MemoryView(FOutputComponents.data(), FOutputComponents.size()); }
    u32 TotalQuantityOfInputResourcesNecessary() const;
    u32 TotalQuantityOfOutputResourcesNecessary() const;

    void DrawInEditor();

    SERIALIZE()
    {
        PROPERTYFIELD(Name, "Default");
        PROPERTYFIELD(InputComponents, std::vector<RecipeComponent>());
        PROPERTYFIELD(OutputComponents, std::vector<RecipeComponent>());
        PROPERTYFIELD(CraftDuration, 0);
    }

private:
    void DrawComponentsInEditor(const std::string& parName, std::vector<RecipeComponent>& parVector);

private:
    std::string FName = std::string("Default");
    std::vector<RecipeComponent> FInputComponents;
    std::vector<RecipeComponent> FOutputComponents;
    float FCraftDuration = 0;
};
} // namespace ECSEngine