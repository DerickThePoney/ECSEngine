#pragma once

#include "ECSCore/ModuleSystem.h"
#include "UIInGameMenuController.h"

namespace ECSEngine
{
class UserInterfaceSystem final : public ModuleSystem
{
public:
    UserInterfaceSystem();
    ~UserInterfaceSystem();

protected:
    void VirtualInit() override;
    void VirtualUpdate() override;
    void VirtualDestroy() override;

private:
    UI::UIInGameMenuController FInGameMenu;
};
} // namespace ECSEngine
