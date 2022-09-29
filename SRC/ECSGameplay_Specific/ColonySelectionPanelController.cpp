#include "stdafx.h"

#include "ColonySelectionPanelController.h"

#include "ColonyModule.h"
#include "ECSCore/ScopedModuleAccessor.h"
#include "ECSCore/WorldIds.h"
#include "GameplayRulesManager.h"
#include "RenderingCore/GLFWDisplayWindowHandler.h"
#include "ResourceStorageModule.h"
#include "imgui/imgui_internal.h"

namespace ECSEngine
{
namespace UI
{

using AccessHelpers = ScopedModuleAccessor<MC<ColonyModule, EEntityWorlds::COLONY>, MC<ResourceStorageModule, EEntityWorlds::COLONY>>;

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

    const float size_x = 0.2f, size_y = 0.4f;
    const float pos_x = 10.f, pos_y = 1.f;
    uvec2 windowSize = Rendering::GLFWDisplayWindowHandler::Instance().GetSize();
    ImGui::SetNextWindowPos(ImVec2(windowSize.x - pos_x, windowSize.y * pos_y), ImGuiCond_Always, ImVec2(1.f, 1.f));
    ImGui::SetNextWindowSize(vec2(windowSize.x * size_x, windowSize.y * size_y));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowTitleAlign, ImVec2(0.5f, 0.5f));
    ImGui::Begin(colonyModule->Name().c_str(), NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove);

    const u32 peonsInColony = 0;

    // Show Basic informations
    if (ImGui::CollapsingHeader("Basic information", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Text("PEONS\t%d", peonsInColony);
        ImGui::Text("Idle PEONS\t%d", 0);

        u32 totalPlace = 0;
        u32 freePlace = 0;

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
        const float progress = 0;
        ImGui::PushID("feedingTimeProgress");
        ImGui::Text("Next feeding time: ");
        ImGui::SameLine();
        const u32 foodInStorage = resourceStorage->GetResourceQuantity(GameResource::FOOD);
        if (foodInStorage < peonsInColony * GameplayConstants::PeonFeeding::PeonEatQuantity)
            ImGui::TextColored(ImVec4(255, 0, 0, 255), "Needs %d food", peonsInColony * GameplayConstants::PeonFeeding::PeonEatQuantity);
        else
            ImGui::Text("Needs %d food", peonsInColony * GameplayConstants::PeonFeeding::PeonEatQuantity);

        ImGui::ProgressBar(progress, ImVec2(-1.f, 0.f), fmt::format("{:.2f}s remaining", 0).c_str());
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

    ImGui::End();

    ImGui::PopStyleVar();
}

} // namespace UI
} // namespace ECSEngine
