#include "stdafx.h"

#include "PeonSpawningRulesManager.h"

#include "ECSCore/ECSCorePropertyDrawer.h"
#include "ECSCore/EntityTemplateManager.h"

namespace ECSEngine
{

const EntityTemplate* PeonSpawningCostRule::PeonTemplate() const
{
    return EntityTemplateManager::Instance().GetEntityTemplate(FPeonTemplateName);
}

std::vector<std::pair<GameResource::Type, u32>>&& PeonSpawningCostRule::CostForNextSpawn(const u32 parCurrentPeonsQuantity) const
{
    std::vector<std::pair<GameResource::Type, u32>> res;

    foreachitemconst(costComputer, FPeonCostComputers) { res.push_back({ costComputer.FResourceToPay, costComputer.GetCostForNextSpawn(parCurrentPeonsQuantity) }); }

    return std::move(res);
}

void PeonSpawningCostRule::DrawEditingHeader()
{
    ImGui::Text("Peon template");
    ImGui::NextColumn();
}

void PeonSpawningCostRule::DrawEditor()
{
    const EntityTemplate* et = PeonTemplate();
    EDITOR_PROPERTY_ENTITY_TEMPLATE_FILTERED("##Template", et, FPeonTemplateName, Worlds::PEONS);
    ImGui::NextColumn();
    foreachitem(peonCost, FPeonCostComputers) peonCost.DrawEditor();
}

std::vector<std::pair<GameResource::Type, u32>>&& PeonSpawningRulesManager::GetCostForNextSpawn(const std::string& parPeonName, const u32 parCurrentPeonsQuantity)
{
    forrange(i, 0, FPeonSpawningRules.size())
    {
        if (FPeonSpawningRules[i].PeonTemplateName() == parPeonName)
        {
            return GetCostForNextSpawn((u32)i, parCurrentPeonsQuantity);
        }
    }
    AssertNotReached();
    return std::move(std::vector<std::pair<GameResource::Type, u32>>());
}

std::vector<std::pair<GameResource::Type, u32>>&& PeonSpawningRulesManager::GetCostForNextSpawn(const u32 parTemplateIndex, const u32 parCurrentPeonsQuantity)
{
    AssertRelease(parTemplateIndex < (u32)FPeonSpawningRules.size());
    return FPeonSpawningRules[parTemplateIndex].CostForNextSpawn(parCurrentPeonsQuantity);
}

void PeonSpawningRulesManager::DrawEditor()
{
    // Table with Peon name, cost, multiplier all this stuff
    u32 maxCostNumber = 0;
    foreachitemconst(costRule, FPeonSpawningRules) { maxCostNumber = std::max(maxCostNumber, costRule.CostRulesNumber()); }
    u32 columnsNumber = 3 * maxCostNumber;
    columnsNumber = std::min(columnsNumber + 1, 64u);
    columnsNumber = columnsNumber - columnsNumber % 3;

    if (ImGui::Button("Add Cost Rule"))
    {
        PeonSpawningCostRule newRule;
        newRule.FPeonCostComputers.push_back(PeonCostComputer());
        FPeonSpawningRules.push_back(newRule);
    }

    ImGui::PushID(ImGui::GetID(this));
    ImGui::Columns(columnsNumber + 1, "##CostEditor");
    // drawHeader
    PeonSpawningCostRule::DrawEditingHeader();
    ImGui::Separator();

    forrange(i, 0, maxCostNumber) PeonCostComputer::DrawEditingHeader();
    ImGui::Separator();

    // Draw cost computer
    forrange(i, 0, FPeonSpawningRules.size())
    {
        ImGui::PushID((int)i);
        FPeonSpawningRules[i].DrawEditor();
        ImGui::PopID();
    }

    ImGui::Columns(1);

    ImGui::PopID();
}

} // namespace ECSEngine