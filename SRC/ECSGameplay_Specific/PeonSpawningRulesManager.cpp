#include "stdafx.h"

#include "PeonSpawningRulesManager.h"

#include "ECSCore/ECSCorePropertyDrawer.h"
#include "ECSCore/EntityTemplateManager.h"
#include "imgui/imgui_internal.h"

namespace ECSEngine
{

const EntityTemplate* PeonSpawningCostRule::PeonTemplate() const
{
    return EntityTemplateManager::Instance().GetEntityTemplate(FPeonTemplateName);
}

std::vector<std::pair<GameResource::Type, u32>> PeonSpawningCostRule::CostForNextSpawn(const u32 parCurrentPeonsQuantity) const
{
    std::vector<std::pair<GameResource::Type, u32>> res;

    foreachitemconst(costComputer, FPeonCostComputers) { res.push_back({ costComputer.FResourceToPay, costComputer.GetCostForNextSpawn(parCurrentPeonsQuantity) }); }

    return res;
}

void PeonSpawningCostRule::DrawEditingHeader()
{
    ImGui::Text("Peon template");
    ImGui::NextColumn();
    ImGui::Text("Cost rules");
    ImGui::NextColumn();
}

void PeonSpawningCostRule::DrawEditor()
{
    const EntityTemplate* et = PeonTemplate();
    EDITOR_PROPERTY_ENTITY_TEMPLATE_FILTERED("##Template", et, FPeonTemplateName, Worlds::PEONS);
    ImGui::NextColumn();
    ImGui::BeginChild(ImGui::GetID(this), ImVec2(ImGui::GetContentRegionAvailWidth(), 100.f));
    ImGui::Columns(3);
    ImGui::Separator();
    PeonCostComputer::DrawEditingHeader();
    ImGui::Separator();
    foreachitem(peonCost, FPeonCostComputers) peonCost.DrawEditor();
    ImGui::Columns(1);
    ImGui::EndChild();
    ImGui::NextColumn();
}

std::vector<std::pair<GameResource::Type, u32>> PeonSpawningRulesManager::GetCostForNextSpawn(const std::string& parPeonName, const u32 parCurrentPeonsQuantity)
{
    forrange(i, 0, FPeonSpawningRules.size())
    {
        if (FPeonSpawningRules[i].PeonTemplateName() == parPeonName)
        {
            return GetCostForNextSpawn((u32)i, parCurrentPeonsQuantity);
        }
    }
    AssertNotReached();
    return std::vector<std::pair<GameResource::Type, u32>>();
}

std::vector<std::pair<GameResource::Type, u32>> PeonSpawningRulesManager::GetCostForNextSpawn(const u32 parTemplateIndex, const u32 parCurrentPeonsQuantity)
{
    AssertRelease(parTemplateIndex < (u32)FPeonSpawningRules.size());
    return FPeonSpawningRules[parTemplateIndex].CostForNextSpawn(parCurrentPeonsQuantity);
}

void PeonSpawningRulesManager::DrawEditor()
{
    // Table with Peon name, cost, multiplier all this stuff
    static u32 selected = -1;

    if (ImGui::Button("Add Cost Rule"))
    {
        PeonSpawningCostRule newRule;
        newRule.FPeonCostComputers.push_back(PeonCostComputer());
        FPeonSpawningRules.push_back(newRule);
    }
    ImGui::SameLine();
    const bool disabled = selected == -1;
    if (disabled)
    {
        ImGui::PushItemFlag(ImGuiItemFlags_ReadOnly, true);
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
    }
    if (ImGui::Button((disabled) ? "Select the rule you want to delete" : "Delete selected rule"))
    {
        AssertRelease(selected < (u32)FPeonSpawningRules.size());
        FPeonSpawningRules.erase(FPeonSpawningRules.begin() + selected);
    }

    if (disabled)
    {
        ImGui::PopItemFlag();
        ImGui::PopStyleVar();
    }
    ImGui::SameLine();
    ImGui::Checkbox("Auto spawn", &FAutoSpawn);

    ImGui::PushID(ImGui::GetID(this));
    ImGui::Columns(2, "##CostEditor");
    // drawHeader

    PeonSpawningCostRule::DrawEditingHeader();
    ImGui::Separator();

    // Draw cost computer
    forrange(i, 0, FPeonSpawningRules.size())
    {
        ImGui::PushID((int)i);
        if (ImGui::Selectable(fmt::format("{} - ", i).c_str(), (u32)i == selected, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowItemOverlap, ImVec2(0.f, 200.f)))
            selected = (u32)i;
        ImGui::SameLine();
        FPeonSpawningRules[i].DrawEditor();
        ImGui::PopID();
        ImGui::Separator();
    }

    ImGui::Columns(1);
    ImGui::Separator();

    ImGui::PopID();
}

} // namespace ECSEngine
