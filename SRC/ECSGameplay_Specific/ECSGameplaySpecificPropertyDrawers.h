#pragma once
#include "BuildingCostManager.h"
#include "ECSCore/ECSCorePropertyDrawer.h"
#include "GameResources.h"
#include "GameplayRulesManager.h"
#include "ProductionRecipe.h"
#include "ProductionRecipesManager.h"

namespace ECSEngine
{
template<>
class PropertyDrawer<GameResource::Type>
{
public:
    PropertyDrawer(const std::string& parPropertyName, GameResource::Type* parProperty, bool parAllowLength = false)
        : FPropertyName(parPropertyName)
        , FProperty(parProperty)
        , FAllowLength(parAllowLength)
    {
    }

    void ShowProperty()
    {
        u32 lastIndex = (FAllowLength) ? GameResource::LENGTH + 1 : GameResource::LENGTH;

        if (!FPropertyName.empty())
        {
            ImGui::Text(FPropertyName.c_str());
            ImGui::SameLine();
        }

        const vec2 availableSize = ImGui::GetContentRegionAvail();
        ImGui::SetNextItemWidth(availableSize.x * 0.4f);
        if (ImGui::BeginCombo(std::format("##{}", FPropertyName).c_str(), GameResource::GetName(*FProperty)))
        {
            forrange(i, 0, lastIndex)
            {
                if (ImGui::Selectable(GameResource::GetName((GameResource::Type)i), i == *FProperty))
                {
                    *FProperty = (GameResource::Type)i;
                }
            }

            ImGui::EndCombo();
        }
    }

private:
    std::string FPropertyName;
    GameResource::Type* FProperty;
    bool FAllowLength;
};

#define EDITOR_PROPERTY_GAME_RESOURCES(NAME, PROPERTY, ALLOW_LENGTH)                                                                                                               \
    {                                                                                                                                                                              \
        PropertyDrawer<GameResource::Type> drawer(NAME, &PROPERTY, ALLOW_LENGTH);                                                                                                  \
        drawer.ShowProperty();                                                                                                                                                     \
    }

template<>
class PropertyDrawer<BuildingCategory::Type>
{
public:
    PropertyDrawer(const std::string& parPropertyName, BuildingCategory::Type* parProperty)
        : FPropertyName(parPropertyName)
        , FProperty(parProperty)
    {
    }

    void ShowProperty()
    {
        u32 lastIndex = BuildingCategory::LENGTH;

        if (!FPropertyName.empty())
        {
            ImGui::Text(FPropertyName.c_str());
            ImGui::SameLine();
        }

        const vec2 availableSize = ImGui::GetContentRegionAvail();
        ImGui::SetNextItemWidth(availableSize.x * 0.4f);
        if (ImGui::BeginCombo(std::format("##{}", FPropertyName).c_str(), BuildingCategory::AsString(*FProperty)))
        {
            forrange(i, 0, lastIndex)
            {
                if (ImGui::Selectable(BuildingCategory::AsString((BuildingCategory::Type)i), i == *FProperty))
                {
                    *FProperty = (BuildingCategory::Type)i;
                }
            }

            ImGui::EndCombo();
        }
    }

private:
    std::string FPropertyName;
    BuildingCategory::Type* FProperty;
};

#define EDITOR_PROPERTY_BUILDING_CATEGORY(NAME, PROPERTY)                                                                                                                          \
    {                                                                                                                                                                              \
        PropertyDrawer<BuildingCategory::Type> drawer(NAME, &PROPERTY);                                                                                                            \
        drawer.ShowProperty();                                                                                                                                                     \
    }

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
        const vec2 availableSize = ImGui::GetContentRegionAvail();
        ImGui::SetNextItemWidth(availableSize.x * 0.4f);

        EDITOR_PROPERTY_GAME_RESOURCES("Resource", FProperty->first, false);
        EDITOR_PROPERTY_SIMPLE("Quantity", FProperty->second);
    }

    std::string FName;
    RecipeComponent* FProperty = nullptr;
};

#define EDITOR_PROPERTY_RECIPE_COMPONENT(NAME, PROPERTY)                                                                                                                           \
    {                                                                                                                                                                              \
        PropertyDrawer<RecipeComponent> drawer(NAME, &PROPERTY);                                                                                                                   \
        drawer.ShowProperty();                                                                                                                                                     \
    }

template<>
class PropertyDrawer<ProductionRecipe>
{
public:
    PropertyDrawer(const std::string& parPropertyName, const ProductionRecipe** parProperty, std::string* parNameProperty)
        : FPropertyName(parPropertyName)
        , FProperty(parProperty)
        , FNameProperty(parNameProperty)
    {
    }

    void ShowProperty()
    {
        MemoryView<const ProductionRecipe> recipes = GameplayRulesManager::Instance().FProductionRecipesManager.ProductionRecipes();
        u32 lastIndex = recipes.size();

        if (!FPropertyName.empty())
        {
            ImGui::Text(FPropertyName.c_str());
            ImGui::SameLine();
        }

        const vec2 availableSize = ImGui::GetContentRegionAvail();
        ImGui::SetNextItemWidth(availableSize.x * 0.4f);
        if (ImGui::BeginCombo(std::format("##{}", FPropertyName).c_str(), FNameProperty->c_str()))
        {
            forrange(i, 0, lastIndex)
            {
                if (ImGui::Selectable(recipes[i].Name().c_str(), recipes[i].Name() == FNameProperty->c_str()))
                {
                    *FProperty = &recipes[i];
                    *FNameProperty = recipes[i].Name();
                }
            }

            ImGui::EndCombo();
        }
    }

private:
    std::string FPropertyName;
    const ProductionRecipe** FProperty = nullptr;
    std::string* FNameProperty = nullptr;
};

#define EDITOR_PROPERTY_PRODUCTION_RECIPE(NAME, PROPERTY, NAMEPROPERTY)                                                                                                            \
    {                                                                                                                                                                              \
        PropertyDrawer<ProductionRecipe> drawer(NAME, &PROPERTY, &NAMEPROPERTY);                                                                                                   \
        drawer.ShowProperty();                                                                                                                                                     \
    }
} // namespace ECSEngine
