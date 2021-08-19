#pragma once
#include "BuildMenuController.h"
#include "ColonySelectionPanelController.h"
#include "ECSCore/UIController.h"
#include "ECSGameplay_Common/UIWindowsPositionning.h"
#include "MainMenuBarController.h"

namespace ECSEngine
{
namespace UI
{

class UIInGameMenuController : public UIController
{
public:
    UIInGameMenuController();

protected:
    void VirtualInit() override;
    void VirtualUpdate() override;
    void VirtualDestroy() override;

private:
    ColonySelectionPanelController FColonySelectionPanel;
    BuildMenuController FBuildMenuController;
    MainMenuBarController FMainMenuBarController;
};

} // namespace UI
} // namespace ECSEngine
