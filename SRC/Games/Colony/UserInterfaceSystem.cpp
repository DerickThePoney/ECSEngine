#include "stdafx.h"

#include "UserInterfaceSystem.h"

#include "UIInGameMenuController.h"

namespace ECSEngine
{

UserInterfaceSystem::UserInterfaceSystem()
    : ModuleSystem()
{
}

UserInterfaceSystem::~UserInterfaceSystem()
{
}

void UserInterfaceSystem::VirtualUpdate()
{
    ModuleSystem::VirtualUpdate();

    FInGameMenu->Update();
}

void UserInterfaceSystem::VirtualInit()
{
    ModuleSystem::VirtualInit();
    FInGameMenu.reset(new UI::UIInGameMenuController);
    FInGameMenu->Show(true);
    FInGameMenu->Init();
}

void UserInterfaceSystem::VirtualDestroy()
{
    ModuleSystem::VirtualDestroy();
    FInGameMenu->Destroy();
}

} // namespace ECSEngine
