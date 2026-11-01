#include "stdafx.h"

#include "ResourceStorageModule.h"

#include "Application/PropertyDrawer.h"
#include "Common/SavingSystemImplementation.h"
#include "Common/TimeManager.h"
#include "ECSCore/EntityId.h"
#include "ECSCore/EntityTemplateManagerMethods.h"
#include "ECSCore/ModuleParameters.h"
#include "ECSCore/ModuleUtils.h"
#include "ECSGameplaySpecificPropertyDrawers.h"

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
        auto itToErase = FStartingResources.end();
        u32 i = 0;
        for (auto it = FStartingResources.begin(); it != FStartingResources.end(); it++)
        {
            ImGui::PushID(i);
            if (ImGui::Button("X"))
                itToErase = it;
            ImGui::SameLine();

            EDITOR_PROPERTY_GAME_RESOURCES("Resource", it->first, false);
            ImGui::SameLine();

            EDITOR_PROPERTY_WITH_LIMITS("Qty", it->second, 0u, FMaxResourceQuantity);
            ImGui::PopID();
            ++i;
        }

        if (itToErase != FStartingResources.end())
        {
            FStartingResources.erase(itToErase);
        }

        if (ImGui::Button("Add starting resource"))
            FStartingResources.push_back({ GameResource::FOOD, 0 });
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

    if (parResource != GameResource::LENGTH)
        return 0;

    AlwaysCheckedAssert(parResource == GameResource::LENGTH);
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
    const u32 resourceToStore = Min(parQuantity, remainingStorageSpace);
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

    FResourceStatisticsManager.AddResourceChange(parResource, (i32)parQuantity);

    return resourceToStore;
}

u32 ResourceStorageModule::RemoveResource(const GameResource::Type parResource, const u32 parQuantity)
{
    bool found = false;
    foreachitem(resource, FCarriedResources)
    {
        if (resource.first == parResource)
        {
            u32 resRemoved = Min(parQuantity, resource.second);
            resource.second -= resRemoved;
            FResourceStatisticsManager.AddResourceChange(parResource, -((i32)parQuantity));
            return resRemoved;
        }
    }

    return 0;
}

GameResource::Type ResourceStorageModule::GetMainResource() const
{
    i32 maxResourceSize = -1;
    GameResource::Type maxResource = GameResource::LENGTH;

    foreachitemconst(res, FCarriedResources)
    {
        if ((i32)res.second > maxResourceSize)
        {
            maxResourceSize = (i32)maxResourceSize;
            maxResource = res.first;
        }
    }
    return maxResource;
}

IMPLEMENT_SAVELOAD_ABILITIES(ResourceStorageModule);
template<typename Chunk, bool isWriting>
void ResourceStorageModule::SaveLoad(Chunk& parChunk)
{
    parent_type::SaveLoad(parChunk);

    parChunk& FCarriedResources;
}

IMPLEMENT_SAVELOAD_ABILITIES(ResourcesStatistics);
template<typename Chunk, bool isWriting>
void ResourcesStatistics::SaveLoad(Chunk& parChunk)
{
    parChunk& FStatistics;
}

IMPLEMENT_SAVELOAD_ABILITIES(ResourcesStatistics::ResourceData);
template<typename Chunk, bool isWriting>
void ResourcesStatistics::ResourceData::SaveLoad(Chunk& parChunk)
{
    parChunk& Changes;
    parChunk& AverageResourcePerUnitOfTime;
}

void ResourcesStatistics::AddResourceChange(const GameResource::Type parResource, const i32 parQuantity)
{
    auto itFind = FStatistics.find(parResource);
    if (itFind == FStatistics.end())
    {
        ResourceData data;
        data.Changes.push_back({ TimeManager::FrameStartTime(), parQuantity });
        FStatistics.insert_or_assign(parResource, data);
        return;
    }

    itFind->second.Changes.push_back({ TimeManager::FrameStartTime(), parQuantity });
}

float ResourcesStatistics::GetAverageResourcePerUnitOfTime(const GameResource::Type parResource) const
{
    auto itFind = FStatistics.find(parResource);
    if (itFind == FStatistics.end())
        return 0.f;

    return itFind->second.AverageResourcePerUnitOfTime;
}

void ResourcesStatistics::UpdateStatistics(const float parNow)
{
    foreachitem(resStats, FStatistics)
    {
        i32 cumulativeChange = 0;
        // on vire les elements trop loin
        while (!resStats.second.Changes.empty() && parNow - resStats.second.Changes.begin()->first > MaxTimeForMovingAverage)
            resStats.second.Changes.pop_front();

        foreachitemconst(change, resStats.second.Changes) { cumulativeChange += change.second; }

        resStats.second.AverageResourcePerUnitOfTime = cumulativeChange / MaxTimeForMovingAverage;
    }
}

} // namespace ECSEngine
