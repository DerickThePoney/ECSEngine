#pragma once
#include "BuildingCostManager.h"
#include "ECSCore/ECSCorePropertyDrawer.h"
#include "GameResources.h"

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

        const glm::vec2 availableSize = ImGui::GetContentRegionAvail();
        ImGui::SetNextItemWidth(availableSize.x * 0.4f);
        if (ImGui::BeginCombo(fmt::format("##{}", FPropertyName).c_str(), GameResource::GetName(*FProperty)))
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

        const glm::vec2 availableSize = ImGui::GetContentRegionAvail();
        ImGui::SetNextItemWidth(availableSize.x * 0.4f);
        if (ImGui::BeginCombo(fmt::format("##{}", FPropertyName).c_str(), BuildingCategory::AsString(*FProperty)))
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
} // namespace ECSEngine
