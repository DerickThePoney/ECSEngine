#pragma once
#include "BuildMenuController.h"
#include "ColonySelectionPanelController.h"
#include "ECSCore/UIController.h"
#include "ECSGameplay_Common/UIWindowsPositionning.h"

namespace ECSEngine
{
namespace UI
{

struct InGameMenuElements
{
    bool ShowInGameMenu = true;
    bool BuildMenu = false;
    bool ColonySelectionMenu = false; // TOREMOVE JUST FOR THE LOLS
};

class UIInGameMenuController : public UIController
{
public:
    UIInGameMenuController();

protected:
    void VirtualUpdate() override;

private:
    void DrawInGameMenu();

private:
    ColonySelectionPanelController FColonySelectionPanel;
    BuildMenuController FBuildMenuController;

    InGameMenuElements FMenuElements;

    WindowSizer FSize;
    WindowPosition FPosition;
};

} // namespace UI
} // namespace ECSEngine
