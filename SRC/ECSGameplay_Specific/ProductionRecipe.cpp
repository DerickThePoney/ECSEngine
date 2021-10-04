#include "stdafx.h"

#include "ProductionRecipe.h"

#include "Application/PropertyDrawer.h"
#include "ECSGameplaySpecificPropertyDrawers.h"

namespace ECSEngine
{
void ProductionRecipe::DrawInEditor()
{
    EDITOR_PROPERTY_STRING("Name", FName, false, "");

    EDITOR_PROPERTY_WITH_LIMITS("Craft duration", FCraftDuration, 0.f, 1000.f);

    DrawComponentsInEditor("Inputs", FInputComponents);
    DrawComponentsInEditor("Outputs", FOutputComponents);
}

void ProductionRecipe::DrawComponentsInEditor(const std::string& parName, std::vector<RecipeComponent>& parVector)
{
    ImGui::PushID(parName.c_str());
    if (ImGui::CollapsingHeader(fmt::format("{}##VectorPropertyDrawer", parName).c_str()))
    {
        ImGui::Indent();
        auto itToErase = parVector.end();
        u32 i = 0;
        u32 action = -1; // 0 erase / 1 up / 2 down
        for (auto element = parVector.begin(); element != parVector.end(); ++element, ++i)
        {
            ImGui::PushID(i);
            if (ImGui::Button("X"))
            {
                itToErase = element;
                action = 0;
            }
            ImGui::SameLine();
            if (i > 0 && ImGui::Button("UP"))
            {
                itToErase = element;
                action = 1;
            }
            ImGui::SameLine();
            if (i < ((u32)parVector.size() - 1) && ImGui::Button("DOWN"))
            {
                itToErase = element;
                action = 2;
            }
            ImGui::SameLine();

            EDITOR_PROPERTY_RECIPE_COMPONENT(fmt::format("Item_{}", i), parVector[i]);

            ImGui::PopID();
        }
        ImGui::Unindent();

        if (itToErase != parVector.end())
        {
            switch (action)
            {
            case 0:
            {
                parVector.erase(itToErase);
                break;
            }
            case 1:
            {
                auto previousIt = itToErase - 1;
                std::iter_swap(itToErase, previousIt);
                break;
            }
            case 2:
            {
                auto nextIt = itToErase + 1;
                std::iter_swap(itToErase, nextIt);
                break;
            }
            default:
                AssertNotReached();
            }
        }

        if (ImGui::Button(fmt::format("Add {}", parName).c_str()))
        {
            parVector.push_back({ GameResource::LENGTH, 0 });
        }
    }
    ImGui::PopID();
}

} // namespace ECSEngine
