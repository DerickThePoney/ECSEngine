#include "stdafx.h"

#include "ColonyBuildingSystem.h"

#include "CircularBuildingGrid.h"
#include "CircularGridAccessor.h"
#include "Common/GenericMessageIdentifiers.h"
#include "Common/GenericMessageManager.h"
#include "Common/Logger.h"
#include "ConstructBuildingMessage.h"
#include "ECSCore/EntityFactory.h"
#include "ECSCore/EntityTemplateManager.h"
#include "ECSCore/ModuleParameters.h"
#include "GameplayRulesManager.h"
#include "ResourceManager.h"
#include "ECSCore/WorldIds.h"

namespace ECSEngine
{

ColonyBuildingSystem::ColonyBuildingSystem()
    : ModuleSystem()
{
    RegisterDepency<StorageSlotModule>(EEntityWorlds::BUILDINGS);
}

ColonyBuildingSystem::~ColonyBuildingSystem()
{
}

namespace
{
void ProcessMessage(const ConstructBuildingMessage& parMessage)
{
    LOG_GAMEPLAY(parMessage.FTemplateName.c_str());

    // consume the resource if possible
    std::vector<BuildingResourceCost> costs = GameplayRulesManager::Instance().FBuildingCostManager.GetCostsForBuilding(parMessage.FTemplateName);
    AlwaysCheckedAssert(!costs.empty());

    // check that all resources are available and early bail not
    foreachitemconst(cost, costs)
    {
        if (ResourceManager::Instance().GetResourceQuantity(cost.first) < cost.second)
        {
            LOG_GAMEPLAY("Can't afford building...");
            return;
        }
    }

    // now we can consumme
    foreachitemconst(cost, costs)
    {
        ResourceManager::Instance().ConsumeFromAnyStorage(cost.first, cost.second);
    }


    ModuleParameters::ParameterContainer container;
    container.Set<ModuleParameters::Position>(parMessage.FPosition);
    container.Set<ModuleParameters::Orientation>(quat(0.0f, 0.f, 0.f, 1.f));
    CircularGridAccessor accessor = CircularBuildingGrid::Instance().GetAccessorForWorldPosition(parMessage.FPosition);
    AlwaysCheckedAssert(accessor.Valid());
    container.Set<ModuleParameters::GridAccessor>(accessor);

    const EntityTemplate* temp = EntityTemplateManager::Instance().GetEntityTemplate(parMessage.FTemplateName);
    EntityFactory::RequestCreateEntity(temp, container);
}
} // namespace

void ColonyBuildingSystem::VirtualUpdate()
{
    ModuleSystem::VirtualUpdate();
    GenericMessageManager::Instance().ProcessMessages<GenericMessageId::PLACE_BUILDING, ConstructBuildingMessage>(ProcessMessage);

    // draw grid feedback
}

} // namespace ECSEngine
