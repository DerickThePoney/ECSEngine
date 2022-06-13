#include "stdafx.h"

#include "ColonySelectionPanelController.h"

#include "ColonyModule.h"
#include "ColonyPeonsManagementModule.h"
#include "ECSCore/ScopedModuleAccessor.h"
#include "ECSCore/WorldIds.h"
#include "GameplayRulesManager.h"
#include "HousingPlaceModule.h"
#include "PeonFeedingTimeModule.h"
#include "PeonSpawnModule.h"
#include "PeonSpawningRulesManager.h"
#include "RenderingCore/GLFWDisplayWindowHandler.h"
#include "ResourceStorageModule.h"
#include "imgui/imgui_internal.h"

namespace ECSEngine
{
namespace UI
{

using AccessHelpers = ScopedModuleAccessor<MC<ColonyModule, EEntityWorlds::COLONY>,
      MC<ResourceStorageModule, EEntityWorlds::COLONY>,
      MC<PeonSpawnModule, EEntityWorlds::COLONY>,
      MC<ColonyPeonsManagementModule, EEntityWorlds::COLONY>,
      MC<PeonFeedingTimeModule, EEntityWorlds::COLONY>,
      MC<HousingPlaceModule, EEntityWorlds::BUILDINGS>>;

ColonySelectionPanelController::ColonySelectionPanelController()
    : UIController()
{
}

ColonySelectionPanelController::~ColonySelectionPanelController()
{
}

void ColonySelectionPanelController::VirtualUpdate()
{
    UIController::VirtualUpdate();

    AccessHelpers accessHelpers;

    const EntityId colonyId = EntityId((u32)EEntityWorlds::COLONY, 0);

    const ColonyModule* colonyModule = accessHelpers.GetModule<ColonyModule>(colonyId);
    if (colonyModule == nullptr)
        return;

    const ResourceStorageModule* resourceStorage = accessHelpers.GetModule<ResourceStorageModule>(colonyId);
    AssertRelease(resourceStorage != nullptr);

    const ColonyPeonsManagementModule* peonManagerModule = accessHelpers.GetModule<ColonyPeonsManagementModule>(colonyId);
    AssertRelease(peonManagerModule != nullptr);

    PeonSpawnModule* peonSpawnModule = accessHelpers.GetModule<PeonSpawnModule>(colonyId);
    AssertRelease(peonSpawnModule != nullptr);

    const float size_x = 0.2f, size_y = 0.4f;
    const float pos_x = 10.f, pos_y = 1.f;
    glm::uvec2 windowSize = Rendering::GLFWDisplayWindowHandler::Instance().GetSize();
    ImGui::SetNextWindowPos(ImVec2(windowSize.x - pos_x, windowSize.y * pos_y), ImGuiCond_Always, ImVec2(1.f, 1.f));
    ImGui::SetNextWindowSize(glm::vec2(windowSize.x * size_x, windowSize.y * size_y));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowTitleAlign, ImVec2(0.5f, 0.5f));
    ImGui::Begin(colonyModule->Name().c_str(), NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove);

    const u32 peonsInColony = peonManagerModule->PeonsInColony();

    // Show Basic informations
    if (ImGui::CollapsingHeader("Basic information", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Text("PEONS\t%d", peonsInColony);
        ImGui::Text("Idle PEONS\t%d", peonManagerModule->IdlePeons().size());

        u32 totalPlace = 0;
        u32 freePlace = 0;
        foreachitemconst(house, accessHelpers.Accessor<HousingPlaceModule>())
        {
            totalPlace += house.MaxPlace();
            freePlace += house.RemainingFreeSpace();
        }

        if (peonsInColony < totalPlace)
        {
            ImGui::Text("Housing spots: %d/%d", peonsInColony, totalPlace);
        }
        else if (peonsInColony == totalPlace)
        {
            ImGui::TextColored(ImVec4(255, 255, 0, 255), "Housing spots: %d/%d", peonsInColony, totalPlace);
        }
        else
        {
            ImGui::TextColored(ImVec4(255, 0, 0, 255), "Housing spots: %d/%d", peonsInColony, totalPlace);
        }
    }

    // show colony resources
    if (ImGui::CollapsingHeader("Resources", ImGuiTreeNodeFlags_DefaultOpen))
    {
        const PeonFeedingTimeModule* peonFeedingTimeModule = accessHelpers.GetModule<PeonFeedingTimeModule>(colonyId);
        const float progress = peonFeedingTimeModule->RemainTimeBeforeNextFeedAsRatio();
        ImGui::PushID("feedingTimeProgress");
        ImGui::Text("Next feeding time: ");
        ImGui::SameLine();
        const u32 foodInStorage = resourceStorage->GetResourceQuantity(GameResource::FOOD);
        if (foodInStorage < peonsInColony * GameplayConstants::PeonFeeding::PeonEatQuantity)
            ImGui::TextColored(ImVec4(255, 0, 0, 255), "Needs %d food", peonsInColony * GameplayConstants::PeonFeeding::PeonEatQuantity);
        else
            ImGui::Text("Needs %d food", peonsInColony * GameplayConstants::PeonFeeding::PeonEatQuantity);

        ImGui::ProgressBar(progress, ImVec2(-1.f, 0.f), fmt::format("{:.2f}s remaining", peonFeedingTimeModule->RemainingTimeBeforeNextFeed()).c_str());
        ImGui::PopID();

        constexpr static u32 maxResourcesPerColumns = 4;
        u32 drawnResource = 0;
        forrange(i, 0, GameResource::LENGTH)
        {
            if (resourceStorage->GetResourceQuantity((GameResource::Type)i) > 0)
                ++drawnResource;
        }

        // blabla plusieurs resource multiple columns blabla

        forrange(i, 0, GameResource::LENGTH)
        {
            const u32 resource = resourceStorage->GetResourceQuantity((GameResource::Type)i);
            if (resource > 0)
            {
                ImGui::Text("%s\t%d (%.2f/s)", GameResource::GetName((GameResource::Type)i), resource,
                      resourceStorage->Statistics().GetAverageResourcePerUnitOfTime((GameResource::Type)i));
            }
        }
    }

    // show colony actions
    if (ImGui::CollapsingHeader("Actions", ImGuiTreeNodeFlags_DefaultOpen))
    {
        foreachitemconst(spawnRule, GameplayRulesManager::Instance().FPeonSpawningRulesManager.GetCostSpawnRules())
        {
            const std::string& peonTemplateName = spawnRule.PeonTemplateName();

            std::vector<std::pair<GameResource::Type, u32>> costs = spawnRule.CostForNextSpawn(peonManagerModule->PeonsInColony());

            bool disabled = false;
            foreachitemconst(resQ, costs)
            {
                const u32 resourceInStorage = resourceStorage->GetResourceQuantity(resQ.first);
                disabled = disabled || resourceInStorage < resQ.second;
            }

            if (disabled)
            {
                ImGui::PushItemFlag(ImGuiItemFlags_ReadOnly, true);
                ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
            }
            if (ImGui::Button(fmt::format("Spawn {}", peonTemplateName).c_str()) && !disabled)
            {
                peonSpawnModule->RequestPeonSpawn(SpawnPeonOrder(colonyId, spawnRule));
            }
            ImGui::SameLine();

            std::string coststr = "Cost:";

            foreachitemconst(resQ, costs) { coststr = fmt::format("{} {} {} /", coststr, resQ.second, GameResource::GetName(resQ.first)); }

            if (disabled)
                ImGui::TextDisabled(coststr.c_str());
            else
                ImGui::Text(coststr.c_str());

            if (disabled)
            {
                ImGui::PopItemFlag();
                ImGui::PopStyleVar();
            }
        }
    }

    ImGui::End();

    ImGui::PopStyleVar();
}

} // namespace UI
} // namespace ECSEngine
