#include "stdafx.h"

#include "BuildingCostManager.h"

#include "ECSCore/EntityTemplateManager.h"
#include "ECSGameplaySpecificPropertyDrawers.h"
#include "imgui/imgui_internal.h"

namespace ECSEngine
{

const EntityTemplate* BuildingCostDescriptor::BuildingTemplate() const
{
    return EntityTemplateManager::Instance().GetEntityTemplate(FBuildingTemplateName);
}

void BuildingCostDescriptor::DrawEditorHeader()
{
    ImGui::Text("Resource to pay");
    ImGui::NextColumn();

    ImGui::Text("Cost");
    ImGui::NextColumn();
}

void BuildingCostDescriptor::DrawEditor()
{
    const EntityTemplate* et = BuildingTemplate();
    EDITOR_PROPERTY_ENTITY_TEMPLATE_FILTERED("##Template", et, FBuildingTemplateName, Worlds::STANDARD);
    ImGui::NextColumn();
    ImGui::BeginChild(ImGui::GetID(this), ImVec2(ImGui::GetContentRegionAvailWidth(), 100.f));
    ImGui::Columns(2);
    ImGui::Separator();
    DrawEditorHeader();
    ImGui::Separator();
    foreachitem(buildingCost, FCosts)
    {
        ImGui::PushID(ImGui::GetID(&buildingCost));
        EDITOR_PROPERTY_GAME_RESOURCES("", buildingCost.first, true);
        ImGui::NextColumn();
        EDITOR_PROPERTY_WITH_LIMITS("##Cost", buildingCost.second, 0u, 10000u);
        if (buildingCost.first == GameResource::LENGTH)
            buildingCost.second = 0;
        ImGui::NextColumn();
        ImGui::PopID();
    }
    ImGui::Columns(1);
    ImGui::EndChild();
    ImGui::NextColumn();
    if (FCosts.size() == 0)
        FCosts.push_back({ GameResource::LENGTH, 0 });
}

std::vector<BuildingResourceCost> BuildingCostManager::GetCostsForBuilding(const std::string& parBuildingName)
{
    forrange(i, 0, FBuildingsCostRules.size())
    {
        if (parBuildingName == FBuildingsCostRules[i].BuildingTemplateName())
            return GetCostsForBuilding((u32)i);
    }
    AssertNotReached();
    return std::vector<BuildingResourceCost>();
}

std::vector<ECSEngine::BuildingResourceCost> BuildingCostManager::GetCostsForBuilding(const u32 parTemplateIndex)
{
    AssertRelease(parTemplateIndex < (u32)FBuildingsCostRules.size());
    return FBuildingsCostRules[parTemplateIndex].BuildingCosts();
}

void BuildingCostManager::DrawEditor()
{
    // Table with Peon name, cost, multiplier all this stuff
    static u32 selected = -1;

    if (ImGui::Button("Add Cost Rule"))
    {
        BuildingCostDescriptor newDesc;
        FBuildingsCostRules.push_back(newDesc);
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
        AssertRelease(selected < (u32)FBuildingsCostRules.size());
        FBuildingsCostRules.erase(FBuildingsCostRules.begin() + selected);
    }

    if (disabled)
    {
        ImGui::PopItemFlag();
        ImGui::PopStyleVar();
    }

    ImGui::PushID(ImGui::GetID(this));
    ImGui::Columns(2, "##CostEditor");
    // drawHeader

    ImGui::Text("Building template");
    ImGui::NextColumn();
    ImGui::Text("Cost rules");
    ImGui::NextColumn();
    ImGui::Separator();

    // Draw cost computer
    forrange(i, 0, FBuildingsCostRules.size())
    {
        ImGui::PushID((int)i);
        if (ImGui::Selectable(fmt::format("{} - ", i).c_str(), (u32)i == selected, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowItemOverlap, ImVec2(0.f, 200.f)))
            selected = (u32)i;
        ImGui::SameLine();
        FBuildingsCostRules[i].DrawEditor();
        ImGui::PopID();
        ImGui::Separator();
    }

    ImGui::Columns(1);
    ImGui::Separator();

    ImGui::PopID();
}

} // namespace ECSEngine
