#include "stdafx.h"

#include "ResourceProductionModule.h"

#include "Application/PropertyDrawer.h"
#include "ECSCore/EntityId.h"
#include "ECSCore/EntityTemplateManager.h"
#include "ECSCore/ModuleParameters.h"
#include "ECSCore/ModuleUtils.h"
#include "ECSGameplaySpecificPropertyDrawers.h"

CEREAL_REGISTER_TYPE(ECSEngine::ResourceProductionModuleTemplate);
CEREAL_REGISTER_POLYMORPHIC_RELATION(ECSEngine::ModuleTemplate, ECSEngine::ResourceProductionModuleTemplate)

namespace ECSEngine
{
IMPLEMENT_MODULE_TEMPLATE(ResourceProductionModule, ResourceProductionModuleTemplate);

Module* ResourceProductionModuleTemplate::CreateInstance(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) const
{
    return NewModule<ResourceProductionModule>(this, parUnitId, parParameters);
}

void ResourceProductionModuleTemplate::VirtualDrawEditor()
{
    if (ImGui::CollapsingHeader("Produced resources"))
    {
        auto itToErase = FProducedResources.end();
        u32 i = 0;
        for (auto it = FProducedResources.begin(); it != FProducedResources.end(); it++)
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

        if (itToErase != FProducedResources.end())
        {
            FProducedResources.erase(itToErase);
        }

        if (ImGui::Button("Add produced resource"))
            FProducedResources.push_back({ GameResource::FOOD, 10.0f });
    }
}

ResourceProductionModule::ResourceProductionModule()
    : Module()
{
}

void ResourceProductionModule::VirtualInit(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters)
{
    parent_type::VirtualInit(parUnitId, parParameters);

    MemoryView<const ResourceProductionModuleTemplate::ProducedResource> producedResources = Template<ResourceProductionModuleTemplate>()->ProducedResources();

    foreachitemconst(res, producedResources) { FRemainingProductionTimes.push_back({ res.first, res.second }); }
}

void ResourceProductionModule::ResetTimingForResource(const GameResource::Type parResource)
{
    MemoryView<const ResourceProductionModuleTemplate::ProducedResource> producedResources = Template<ResourceProductionModuleTemplate>()->ProducedResources();

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
