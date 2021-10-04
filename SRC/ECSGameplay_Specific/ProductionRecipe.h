#pragma once
#include "Application/PropertyDrawer.h"
#include "Common/MemoryView.h"
#include "ECSGameplaySpecificPropertyDrawers.h"
#include "GameResources.h"

namespace ECSEngine
{
using RecipeComponent = std::pair<GameResource::Type, u32>;

template<>
class PropertyDrawer<RecipeComponent>
{
public:
    PropertyDrawer(const std::string& parPropertyName, RecipeComponent* parProperty)
        : FName(parPropertyName)
        , FProperty(parProperty)
    {
    }

    void ShowProperty()
    {
        const glm::vec2 availableSize = ImGui::GetContentRegionAvail();
        ImGui::SetNextItemWidth(availableSize.x * 0.4f);

        EDITOR_PROPERTY_GAME_RESOURCES("Resource", FProperty->first, false);
        EDITOR_PROPERTY_SIMPLE("Quantity", FProperty->second);
    }

    std::string FName;
    RecipeComponent* FProperty = nullptr;
};

class ProductionRecipe
{
public:
    const std::string& Name() const { return FName; }
    u32 CraftDuration() const { return FCraftDuration; }
    MemoryView<const RecipeComponent> InputComponents() const { return MemoryView(FInputComponents.data(), FInputComponents.size()); }
    MemoryView<const RecipeComponent> OutputComponents() const { return MemoryView(FOutputComponents.data(), FOutputComponents.size()); }

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