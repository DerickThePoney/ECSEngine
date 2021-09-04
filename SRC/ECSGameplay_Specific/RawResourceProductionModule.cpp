#include "stdafx.h"

#include "RawResourceProductionModule.h"

#include "Application/PropertyDrawer.h"
#include "ECSCore/EntityId.h"
#include "ECSCore/EntityTemplateManager.h"
#include "ECSCore/ModuleParameters.h"
#include "ECSCore/ModuleUtils.h"
#include "ECSGameplaySpecificPropertyDrawers.h"

CEREAL_REGISTER_TYPE(ECSEngine::RawResourceProductionModuleTemplate);
CEREAL_REGISTER_POLYMORPHIC_RELATION(ECSEngine::ModuleTemplate, ECSEngine::RawResourceProductionModuleTemplate)

namespace ECSEngine
{
IMPLEMENT_MODULE_TEMPLATE(RawResourceProductionModule, RawResourceProductionModuleTemplate);

Module* RawResourceProductionModuleTemplate::CreateInstance(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) const
{
    return NewModule<RawResourceProductionModule>(this, parUnitId, parParameters);
}

void RawResourceProductionModuleTemplate::VirtualDrawEditor()
{
    if (ImGui::CollapsingHeader("Produced resources"))
    {
        auto itToErase = FProducedRawResources.end();
        u32 i = 0;
        for (auto it = FProducedRawResources.begin(); it != FProducedRawResources.end(); it++)
        {
            ImGui::PushID(i);
            if (ImGui::Button("X"))
                itToErase = it;
            ImGui::SameLine();

            EDITOR_PROPERTY_GAME_RESOURCES("Resource", it->first, false);
            ImGui::SameLine();

            EDITOR_PROPERTY_SIMPLE("Time (s)", it->second);
            ImGui::PopID();
            ++i;
        }

        if (itToErase != FProducedRawResources.end())
        {
            FProducedRawResources.erase(itToErase);
        }

        if (ImGui::Button("Add produced resource"))
            FProducedRawResources.push_back({ GameResource::FOOD, 10.0f });
    }
}

RawResourceProductionModule::RawResourceProductionModule()
    : Module()
{
}

void RawResourceProductionModule::VirtualInit(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters)
{
    parent_type::VirtualInit(parUnitId, parParameters);

    MemoryView<const RawResourceProductionModuleTemplate::ProducedRawResource> producedResources = Template<RawResourceProductionModuleTemplate>()->ProducedRawResources();

    foreachitemconst(res, producedResources) { FRemainingProductionTimes.push_back({ res.first, res.second }); }
}

void RawResourceProductionModule::ResetTimingForResource(const GameResource::Type parResource)
{
    MemoryView<const RawResourceProductionModuleTemplate::ProducedRawResource> producedResources = Template<RawResourceProductionModuleTemplate>()->ProducedRawResources();

    foreachitemconst(res, producedResources)
    {
        if (res.first == parResource)
        {
            foreachitem(resP, FRemainingProductionTimes)
            {
                if (resP.first == parResource)
                {
                    resP.second = res.second;
                    return;
                }
            }
        }
    }

    AssertNotReached();
    return;
}

} // namespace ECSEngine
