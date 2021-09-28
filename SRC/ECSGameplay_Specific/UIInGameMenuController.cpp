#include "stdafx.h"

#include "UIInGameMenuController.h"

#include "RenderingCore/GLFWDisplayWindowHandler.h"

namespace ECSEngine
{
namespace UI
{

UIInGameMenuController::UIInGameMenuController()
{
    FMainMenuBarController.Show(true);
    FBuildMenuController.Show(true);
    FResourcesPanel.Show(true);
}

void UIInGameMenuController::VirtualInit()
{
    UIController::VirtualInit();

    FMainMenuBarController.Init();
    FColonySelectionPanel.Init();
    FBuildMenuController.Init();
    FResourcesPanel.Init();
}

void UIInGameMenuController::VirtualUpdate()
{
    UIController::VirtualUpdate();

    FMainMenuBarController.Update();
    FColonySelectionPanel.Update();
    FBuildMenuController.Update();
    FResourcesPanel.Update();
}

void UIInGameMenuController::VirtualDestroy()
{
    UIController::VirtualDestroy();
    FBuildMenuController.Destroy();
    FColonySelectionPanel.Destroy();
    FMainMenuBarController.Destroy();
    FResourcesPanel.Destroy();
}

} // namespace UI
} // namespace ECSEngine
