#include "stdafx.h"

#include "EnergyEfficiencyDescriptor.h"

namespace ECSEngine
{

float EnergyEfficiencyDescriptor::GetEfficiency(const float parRatio) const
{
    foreachitemconst(value, FValues)
    {
        if (parRatio <= value.first)
            return value.second;
    }
    return 100.f;
}

void EnergyEfficiencyDescriptor::DrawEditor()
{
    ImGui::Indent();
    if (ImGui::CollapsingHeader("Thresholds"))
    {
        ImGui::PushID("EnergyEfficiencyThreshold");
        if (ImGui::Button("Add"))
        {
            if (FValues.empty())
                FValues.push_back({ 0.f, 0.f });
            else
                FValues.push_back(FValues.back());
        }
        ImGui::SameLine();
        if (ImGui::Button("Clear"))
        {
            FValues.clear();
        }

        u32 i = 0;
        auto to_erase = FValues.end();
        auto currentIt = FValues.begin();
        bool shouldSort = false;
        foreachitem(value, FValues)
        {
            ImGui::PushID(i);
            EDITOR_PROPERTY_SIMPLE("##threshold", value.first);
            value.first = Clamp(value.first, 0.f, 1.f);

            ImGui::SameLine();
            EDITOR_PROPERTY_SIMPLE("##efficiency", value.second);
            value.second = Clamp(value.second, 0.f, 100.f);
            ImGui::SameLine();
            if (ImGui::Button("X"))
            {
                to_erase = currentIt;
            }

            ImGui::PopID();
            ++i;
            ++currentIt;
        }

        if (to_erase != FValues.end())
            FValues.erase(to_erase);

        forrange(i, 1, FValues.size())
        {
            if (FValues[i].first < FValues[i - 1].first)
            {
                shouldSort = true;
                break;
            }
        }
        if (shouldSort)
        {
            ImGui::TextColored(vec4(1.f, 0.f, 0.f, 1.f), "ATTENTION: NEEDS SORTING!!!!!!");
            if (ImGui::Button("Sort values"))
            {
                std::sort(FValues.begin(), FValues.end());
            }
        }

        ImGui::PopID();
    }
    ImGui::Unindent();
}

} // namespace ECSEngine
