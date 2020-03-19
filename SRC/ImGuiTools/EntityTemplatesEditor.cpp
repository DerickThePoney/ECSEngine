#include "stdafx.h"

#include "EntityTemplatesEditor.h"

#include "../RenderingCore/GLFWDisplayWindowHandler.h"
#include "ECSCore/EntityTemplateManager.h"

namespace ECSEngine
{
namespace ImGUITools
{

void DrawEntityTemplatesEditor()
{
    AssertRelease(EntityTemplateManager::HasInstance());
    const u32 templatesToDraw = EntityTemplateManager::Instance().GetEntityTemplatesNumber();

    glm::uvec2 windowSize = Rendering::GLFWDisplayWindowHandler::Instance().GetSize();
    ImGui::SetNextWindowSize(windowSize);
    ImGui::SetNextWindowPos(glm::vec2(0.0f));

    ImGui::Begin("Entity templates editor", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
    const glm::vec2 currentWindowSize = ImGui::GetWindowSize();
    const glm::vec2 utilityPlace = currentWindowSize - 50.0f;
    const float listProportion = 0.30f;
    const glm::vec2 listSize = utilityPlace * glm::vec2(listProportion, 1.0f);
    ImGui::SetCursorPosX(((currentWindowSize - utilityPlace) * 0.5f).x);
    ImGui::BeginChild(ImGui::GetID("Entity templates list"), listSize, true, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
    static u32 selected = -1;
    forrange(i, 0, templatesToDraw)
    {
        const EntityTemplate* et = EntityTemplateManager::Instance().GetEntityTemplate(i);
        if (ImGui::Selectable(et->GetName().c_str(), selected == i))
            selected = (u32)i;
    }
    ImGui::EndChild();

    ImGui::SameLine();

    const EntityTemplate* et = (selected != -1) ? EntityTemplateManager::Instance().GetEntityTemplate(selected) : nullptr;
    const glm::vec2 editorPlace = utilityPlace * glm::vec2(1 - listProportion, 1.0f);
    ImGui::BeginChild(ImGui::GetID("Entity template editor"), editorPlace, true, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
    if (et != nullptr)
    {
        ImGui::TextColored(glm::vec4(0.8f, 0.8f, 0.8f, 1.0f), "Entity template name:");
        ImGui::SameLine(0.0f, 5.0f);
        ImGui::Text(et->GetName().c_str());
    }
    ImGui::EndChild();
    ImGui::End();
}

} // namespace ImGUITools
} // namespace ECSEngine
