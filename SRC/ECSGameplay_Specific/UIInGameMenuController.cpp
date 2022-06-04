#include "stdafx.h"

#include "UIInGameMenuController.h"

#include "BuildMenuController.h"
#include "ColonySelectionPanelController.h"
#include "MainMenuBarController.h"
#include "ResourcePanelController.h"

namespace ECSEngine
{
namespace UI
{

UIInGameMenuController::UIInGameMenuController()
{
    FMainMenuBarController.reset(new MainMenuBarController);
    FBuildMenuController.reset(new BuildMenuController);
    FColonySelectionPanel.reset(new ColonySelectionPanelController);
    FResourcesPanel.reset(new ResourcePanelController);

    FMainMenuBarController->Show(true);
    FBuildMenuController->Show(true);
    FResourcesPanel->Show(true);
}

UIInGameMenuController::~UIInGameMenuController()
{
}

void UIInGameMenuController::VirtualInit()
{
    UIController::VirtualInit();

    FMainMenuBarController->Init();
    FColonySelectionPanel->Init();
    FBuildMenuController->Init();
    FResourcesPanel->Init();
}

void UIInGameMenuController::VirtualUpdate()
{
    UIController::VirtualUpdate();

    FMainMenuBarController->Update();
    FColonySelectionPanel->Update();
    FBuildMenuController->Update();
    FResourcesPanel->Update();
}

void UIInGameMenuController::VirtualDestroy()
{
    UIController::VirtualDestroy();
    FBuildMenuController->Destroy();
    FColonySelectionPanel->Destroy();
    FMainMenuBarController->Destroy();
    FResourcesPanel->Destroy();
}

} // namespace UI
} // namespace ECSEngine
