#include "stdafx.h"

#include "UserInterfaceSystem.h"

namespace ECSEngine
{

UserInterfaceSystem::UserInterfaceSystem()
    : ModuleSystem()
{
    FInGameMenu.Show(true);
}

UserInterfaceSystem::~UserInterfaceSystem()
{
}

void UserInterfaceSystem::VirtualUpdate()
{
    ModuleSystem::VirtualUpdate();

    FInGameMenu.Update();
}

void UserInterfaceSystem::VirtualInit()
{
    ModuleSystem::VirtualInit();
    FInGameMenu.Init();
}

void UserInterfaceSystem::VirtualDestroy()
{
    ModuleSystem::VirtualDestroy();
    FInGameMenu.Destroy();
}

} // namespace ECSEngine
