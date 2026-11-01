#pragma once

#include "ECSCore/UIController.h"

namespace ECSEngine
{
namespace UI
{

class ColonySelectionPanelController;
class BuildMenuController;
class MainMenuBarController;
class ResourcePanelController;
class BuildingSelectionPanelController;

class UIInGameMenuController : public UIController
{
public:
    UIInGameMenuController();
    ~UIInGameMenuController();

protected:
    void VirtualInit() override;
    void VirtualUpdate() override;
    void VirtualDestroy() override;

private:
    std::unique_ptr<ColonySelectionPanelController> FColonySelectionPanel;
    std::unique_ptr<BuildMenuController> FBuildMenuController;
    std::unique_ptr<MainMenuBarController> FMainMenuBarController;
    std::unique_ptr<ResourcePanelController> FResourcesPanel;
    std::unique_ptr<BuildingSelectionPanelController> FBuildingSelectionPanel;
};

} // namespace UI
} // namespace ECSEngine
