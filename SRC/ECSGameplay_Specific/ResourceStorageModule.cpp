#include "stdafx.h"

#include "ResourceStorageModule.h"

#include "Application/PropertyDrawer.h"
#include "ECSCore/EntityId.h"
#include "ECSCore/EntityTemplateManager.h"
#include "ECSCore/ModuleParameters.h"
#include "ECSCore/ModuleUtils.h"

CEREAL_REGISTER_TYPE(ECSEngine::ResourceStorageModuleTemplate);
CEREAL_REGISTER_POLYMORPHIC_RELATION(ECSEngine::ModuleTemplate, ECSEngine::ResourceStorageModuleTemplate)

namespace ECSEngine
{
IMPLEMENT_MODULE_TEMPLATE(ResourceStorageModule, ResourceStorageModuleTemplate);

Module* ResourceStorageModuleTemplate::CreateInstance(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) const
{
    return NewModule<ResourceStorageModule>(this, parUnitId, parParameters);
}

void ResourceStorageModuleTemplate::VirtualDrawEditor()
{
    EDITOR_PROPERTY_SIMPLE("Max resources quantity", FMaxResourceQuantity);

    if (ImGui::CollapsingHeader("Starting resources"))
    {
        ImGui::Indent();
        auto itToErase = FStartingResources.end();
        u32 i = 0;
        for (auto it = FStartingResources.begin(); it != FStartingResources.end(); it++)
        {
            ImGui::PushID(i);
            if (ImGui::Button("X"))
                itToErase = it;
            ImGui::SameLine();

            ImGui::Text("%s", GameResource::GetName(it->first));
            ImGui::SameLine();

            int val = it->first;
            ImGui::InputInt("##RESNAME", &val);
            val = glm::clamp(val, 0, (int)GameResource::LENGTH - 1);
            it->first = (GameResource::Type)val;
            ImGui::SameLine();

            ImGui::Text("Qty");
            ImGui::SameLine();

            int val2 = it->second;
            ImGui::InputInt("##Qty", &val2);
            val2 = glm::clamp(val2, 0, (int)FMaxResourceQuantity);
            it->second = (u32)val2;
            ImGui::PopID();
            ++i;
        }

        if (itToErase != FStartingResources.end())
        {
            FStartingResources.erase(itToErase);
        }

        if (ImGui::Button("Add starting resource"))
            FStartingResources.push_back({ GameResource::FOOD, 0 });

        ImGui::Unindent();
    }
}
#ifdef PERFORM_SECURITY_CHECKS
void ResourceStorageModuleTemplate::VirtualVerifyTemplate() const
{
    u32 sumStartResources = 0;
    foreachitemconst(r, FStartingResources)
    {
        AssertRelease(r.first != GameResource::LENGTH);
        sumStartResources += r.second;
    }
    AssertRelease(sumStartResources <= FMaxResourceQuantity);
}
#endif

ResourceStorageModule::ResourceStorageModule()
    : Module()
{
}

void ResourceStorageModule::VirtualInit(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters)
{
    parent_type::VirtualInit(parUnitId, parParameters);

    MemoryView<const ResourceStorageModuleTemplate::StartingResources> startingRes = Template<ResourceStorageModuleTemplate>()->GetStartingResources();

    foreachitemconst(res, startingRes) { AddResource(res.first, res.second); }
}

u32 ResourceStorageModule::GetResourceQuantity(const GameResource::Type parResource) const
{
    u32 sum = 0;
    foreachitemconst(resource, FCarriedResources)
    {
        if (parResource == resource.first)
            return resource.second;
        sum += resource.second;
    }

    AlwaysCheckedAssert(parResource == GameResource::LENGTH);
    if (parResource != GameResource::LENGTH)
        return 0;

    return sum;
}

u32 ResourceStorageModule::GetRemainingStorageSpace() const
{
    const u32 resourceQuantity = GetResourceQuantity(GameResource::LENGTH);
    const u32 maxAllowedResources = Template<ResourceStorageModuleTemplate>()->MaxResourceQuantity();
    AlwaysCheckedAssert(resourceQuantity <= maxAllowedResources);
    return maxAllowedResources - resourceQuantity;
}

u32 ResourceStorageModule::AddResource(const GameResource::Type parResource, const u32 parQuantity)
{
    const u32 remainingStorageSpace = GetRemainingStorageSpace();
    const u32 resourceToStore = glm::min(parQuantity, remainingStorageSpace);
    bool found = false;
    foreachitem(resource, FCarriedResources)
    {
        if (resource.first == parResource)
        {
            resource.second += resourceToStore;
            found = true;
            break;
        }
    }

    if (!found)
        FCarriedResources.push_back({ parResource, resourceToStore });

    return resourceToStore;
}

u32 ResourceStorageModule::RemoveResource(const GameResource::Type parResource, const u32 parQuantity)
{
    bool found = false;
    foreachitem(resource, FCarriedResources)
    {
        if (resource.first == parResource)
        {
            u32 resRemoved = glm::min(parQuantity, resource.second);
            resource.second -= resRemoved;
            return resRemoved;
        }
    }

    return 0;
}

} // namespace ECSEngine
