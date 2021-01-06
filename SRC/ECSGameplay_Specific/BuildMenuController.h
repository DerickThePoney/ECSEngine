#pragma once
#include "ECSCore/UIController.h"
#include "ECSGameplay_Common/UIWindowsPositionning.h"

namespace ECSEngine
{
namespace UI
{
class BuildMenuController : public UIController
{
public:
    BuildMenuController();

protected:
    void VirtualUpdate() override;

private:
    bool FIsPlacingBuilding = false;

    WindowSizer FSize;
    WindowPosition FPosition;
};
} // namespace UI
} // namespace ECSEngine
