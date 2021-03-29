#include "stdafx.h"

#include "UIInGameMenuController.h"

#include "RenderingCore/GLFWDisplayWindowHandler.h"

namespace ECSEngine
{
namespace UI
{

UIInGameMenuController::UIInGameMenuController()
    : FSize(0.4f, 0.1f)
    , FPosition(0.5f, 1.f)
{
    FPosition.WindowAnchor = glm::vec2(0.5f, 1.f);
}

void UIInGameMenuController::VirtualUpdate()
{
    UIController::VirtualUpdate();

    DrawInGameMenu();

    FColonySelectionPanel.Update();
    FBuildMenuController.Update();
}

void UIInGameMenuController::DrawInGameMenu()
{
    glm::uvec2 windowSize = Rendering::GLFWDisplayWindowHandler::Instance().GetSize();
    FPosition.PushPos(windowSize);
    FSize.PushSize(windowSize);

    ImGui::Begin(
          "InGameMenu", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBackground);

    ImGui::Separator();
    if (ImGui::Button("Colony selection panel"))
    {
        FMenuElements.ColonySelectionMenu = !FMenuElements.ColonySelectionMenu;
    }
    ImGui::SameLine();
    if (ImGui::Button("Build menu"))
    {
        FMenuElements.BuildMenu = !FMenuElements.BuildMenu;
    }

    ImGui::End();

    FColonySelectionPanel.Show(FMenuElements.ColonySelectionMenu);
    FBuildMenuController.Show(FMenuElements.BuildMenu);
}

} // namespace UI
} // namespace ECSEngine
