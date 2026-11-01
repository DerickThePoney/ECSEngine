#include "stdafx.h"

#include "BuildingGridOccupancyModule.h"
#include "ColonyModule.h"
#include "ECSCore/EntityWorld.h"
#include "ECSCore/WorldIds.h"
#include "ECSCore/WorldManager.h"
#include "ECSGameplay_Common/ApparenceModule.h"
#include "ECSGameplay_Common/OrientationModule.h"
#include "ECSGameplay_Common/PositionModule.h"
#include "EnergyConsumerModule.h"
#include "EnergyProducerModule.h"
#include "LinkToStorageModule.h"
#include "RecipeProductionModule.h"
#include "ResourceStorageModule.h"
#include "StorageSlotModule.h"
#include "brigand/algorithms/for_each.hpp"
#include "brigand/sequences/list.hpp"

namespace ECSEngine
{
using ResourceProducerControllers = brigand::list<ECSEngine::PositionModule, ECSEngine::OrientationModule, ECSEngine::ApparenceModule, ECSEngine::ResourceStorageModule>;

using ColonyControllers = brigand::list<ECSEngine::PositionModule, ECSEngine::ColonyModule, ECSEngine::ResourceStorageModule>;

using BuildingControllers = brigand::list<ECSEngine::PositionModule,
      ECSEngine::OrientationModule,
      ECSEngine::ApparenceModule,
      ECSEngine::ResourceStorageModule,
      ECSEngine::RecipeProductionModule,
      ECSEngine::BuildingGridOccupancyModule,
      ECSEngine::EnergyProducerModule,
      ECSEngine::EnergyConsumerModule,
      ECSEngine::LinkToStorageModule,
      ECSEngine::StorageSlotModule>;

struct AddController
{
    template<typename U>
    void operator()(brigand::type_<U>)
    {
        world->AddController<U>();
    }

    EntityWorld* world;
};

void CreateColonyWorlds()
{
    {
        EntityWorld* world = new EntityWorld(EEntityWorlds::COLONY);
        auto r = brigand::for_each<ColonyControllers>(AddController{ world });
        WorldManager::Instance().AddEntityWorldStealOwnership(EEntityWorlds::COLONY, world);
    }

    {
        EntityWorld* world = new EntityWorld(EEntityWorlds::RESOURCE_PROD);
        auto r = brigand::for_each<ResourceProducerControllers>(AddController{ world });
        WorldManager::Instance().AddEntityWorldStealOwnership(EEntityWorlds::RESOURCE_PROD, world);
    }

    {
        EntityWorld* world = new EntityWorld(EEntityWorlds::BUILDINGS);
        auto r = brigand::for_each<BuildingControllers>(AddController{ world });
        WorldManager::Instance().AddEntityWorldStealOwnership(EEntityWorlds::BUILDINGS, world);
    }
}
} // namespace ECSEngine
