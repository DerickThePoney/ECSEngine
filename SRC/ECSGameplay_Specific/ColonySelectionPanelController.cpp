#include "stdafx.h"

#include "ColonySelectionPanelController.h"

#include "ColonyModule.h"
#include "RenderingCore/GLFWDisplayWindowHandler.h"
#include "imgui/imgui_internal.h"

namespace ECSEngine
{
namespace UI
{

ColonySelectionPanelController::ColonySelectionPanelController()
{
}

ColonySelectionPanelController::~ColonySelectionPanelController()
{
}

void ColonySelectionPanelController::VirtualUpdate()
{
    UIController::VirtualUpdate();

    const EntityId colonyId = EntityId(Worlds::COLONY, 0);

    const ColonyModule* colonyModule = GetModule<ColonyModule>(colonyId);
    if (colonyModule == nullptr)
        return;

    const ResourceStorageModule* resourceStorage = GetModule<ResourceStorageModule>(colonyId);
    AssertRelease(resourceStorage != nullptr);

    const ColonyPeonsManagementModule* peonManagerModule = GetModule<ColonyPeonsManagementModule>(colonyId);
    AssertRelease(peonManagerModule != nullptr);

    PeonSpawnModule* peonSpawnModule = GetModule<PeonSpawnModule>(colonyId);
    AssertRelease(peonSpawnModule != nullptr);

    const float size_x = 0.2f, size_y = 0.4f;
    const float pos_x = 10.f, pos_y = 1.f;
    glm::uvec2 windowSize = Rendering::GLFWDisplayWindowHandler::Instance().GetSize();
    ImGui::SetNextWindowPos(ImVec2(windowSize.x - pos_x, windowSize.y * pos_y), ImGuiCond_Always, ImVec2(1.f, 1.f));
    ImGui::SetNextWindowSize(glm::vec2(windowSize.x * size_x, windowSize.y * size_y));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowTitleAlign, ImVec2(0.5f, 0.5f));
    ImGui::Begin(colonyModule->Name().c_str(), NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove);

    // Show Basic informations
    if (ImGui::CollapsingHeader("Basic information", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Text("PEONS\t%d", peonManagerModule->PeonsInColony());
    }

    // show colony resources
    if (ImGui::CollapsingHeader("Resources", ImGuiTreeNodeFlags_DefaultOpen))
    {
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
                ImGui::Text("%s\t%d", GameResource::GetName((GameResource::Type)i), resource);
            }
        }
    }

    // show colony actions
    if (ImGui::CollapsingHeader("Actions", ImGuiTreeNodeFlags_DefaultOpen))
    {
        const GameResource::Type resourceToPay = peonSpawnModule->ResourceToPay();
        const u32 costForNextSpawn = peonSpawnModule->CostForNextSpawn(peonManagerModule->PeonsInColony());
        AssertRelease(peonSpawnModule->PeonTemplate() != nullptr);
        const std::string& peonTemplateName = peonSpawnModule->PeonTemplate()->GetName();
        const u32 resourceInStorage = resourceStorage->GetResourceQuantity(resourceToPay);

        const bool disabled = resourceInStorage < costForNextSpawn;
        if (disabled)
        {
            ImGui::PushItemFlag(ImGuiItemFlags_ReadOnly, true);
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
        }
        if (ImGui::Button(fmt::format("Spawn {}", peonTemplateName).c_str()) && !disabled)
        {
            peonSpawnModule->SetRequestedPawnCreation(true);
        }
        ImGui::SameLine();
        if (disabled)
            ImGui::TextDisabled("Cost: %d %s", costForNextSpawn, GameResource::GetName(resourceToPay));
        else
            ImGui::Text("Cost: %d %s", costForNextSpawn, GameResource::GetName(resourceToPay));

        if (disabled)
        {
            ImGui::PopItemFlag();
            ImGui::PopStyleVar();
        }
    }

    ImGui::End();

    ImGui::PopStyleVar();
}

} // namespace UI
} // namespace ECSEngine