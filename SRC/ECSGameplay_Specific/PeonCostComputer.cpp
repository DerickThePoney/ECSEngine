#include "stdafx.h"

#include "PeonCostComputer.h"

#include "Application/PropertyDrawer.h"
#include "ECSGameplaySpecificPropertyDrawers.h"

namespace ECSEngine
{
u32 PeonCostComputer::GetCostForNextSpawn(const u32 parCurrentPeonsQuantity) const
{
    return (u32)floor((float)FBaseCost * pow(FMultiplier, (float)parCurrentPeonsQuantity));
}

void PeonCostComputer::DrawEditingHeader()
{
    ImGui::Text("Base cost");
    ImGui::NextColumn();

    ImGui::Text("Cost multiplier");
    ImGui::NextColumn();

    ImGui::Text("Resource to pay");
    ImGui::NextColumn();
}

void PeonCostComputer::DrawEditor()
{
    ImGui::PushID(ImGui::GetID(this));
    EDITOR_PROPERTY_WITH_LIMITS("##BaseCost", FBaseCost, 0u, 1000u);
    ImGui::NextColumn();
    EDITOR_PROPERTY_WITH_LIMITS("##Multiplier", FMultiplier, 0.f, 2.f);
    ImGui::NextColumn();
    EDITOR_PROPERTY_GAME_RESOURCES("", FResourceToPay, false);
    ImGui::NextColumn();
    ImGui::PopID();
}

} // namespace ECSEngine
