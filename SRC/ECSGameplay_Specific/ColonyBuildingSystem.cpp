#include "stdafx.h"

#include "ColonyBuildingSystem.h"

#include "Common/GenericMessageIdentifiers.h"
#include "Common/GenericMessageManager.h"
#include "Common/Logger.h"
#include "ConstructBuildingMessage.h"

namespace ECSEngine
{

ColonyBuildingSystem::ColonyBuildingSystem()
    : ModuleSystem()
{
}

ColonyBuildingSystem::~ColonyBuildingSystem()
{
}

namespace
{
void ProcessMessage(const ConstructBuildingMessage& parMessage)
{
    LOG_GAMEPLAY(parMessage.FTemplateName.c_str());
}
} // namespace

void ColonyBuildingSystem::VirtualUpdate()
{
    ModuleSystem::VirtualUpdate();
    GenericMessageManager::Instance().ProcessMessages<GenericMessageId::PLACE_BUILDING, ConstructBuildingMessage>(ProcessMessage);

    // draw grid feedback
}

} // namespace ECSEngine
