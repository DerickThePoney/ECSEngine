#include "stdafx.h"

#include "UserInterfaceSystem.h"

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

    FSelectionPanel.Update();
}

} // namespace ECSEngine
