#include "stdafx.h"

#include "ProductionRecipesManager.h"

#include "ProductionRecipe.h"

namespace ECSEngine
{

void ProductionRecipesManager::DrawInEditor()
{
    ImGui::Indent();
    auto itToErase = FRecipes.end();
    u32 i = 0;
    u32 action = -1; // 0 erase / 1 up / 2 down
    for (auto element = FRecipes.begin(); element != FRecipes.end(); ++element, ++i)
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
        if (i < ((u32)FRecipes.size() - 1) && ImGui::Button("DOWN"))
        {
            itToErase = element;
            action = 2;
        }
        ImGui::SameLine();

        FRecipes[i].DrawInEditor();
        ImGui::Separator();
        ImGui::PopID();
    }
    ImGui::Unindent();

    if (itToErase != FRecipes.end())
    {
        switch (action)
        {
        case 0:
        {
            FRecipes.erase(itToErase);
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
    ImGui::Separator();
    if (ImGui::Button(std::format("Add Recipe").c_str()))
    {
        FRecipes.push_back(ProductionRecipe());
    }
}

const ProductionRecipe* ProductionRecipesManager::GetProductionRecipe(const std::string& parName) const
{
    foreachitemconst(recipe, FRecipes)
    {
        if (recipe.Name() == parName)
            return &recipe;
    }
    return nullptr;
}

} // namespace ECSEngine