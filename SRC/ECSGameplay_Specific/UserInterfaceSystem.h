#pragma once

#include "ECSCore/ModuleSystem.h"

namespace ECSEngine
{
namespace UI
{
class UIInGameMenuController;
}
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
    std::unique_ptr<UI::UIInGameMenuController> FInGameMenu;
};
} // namespace ECSEngine
