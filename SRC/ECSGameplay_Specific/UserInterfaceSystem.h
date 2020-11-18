#pragma once

#include "ColonySelectionPanelController.h"
#include "ECSCore/ModuleSystem.h"

namespace ECSEngine
{
class UserInterfaceSystem final : public ModuleSystem
{
public:
    UserInterfaceSystem();
    ~UserInterfaceSystem();

protected:
    void VirtualUpdate() override;

private:
    UI::ColonySelectionPanelController FSelectionPanel;
};
} // namespace ECSEngine