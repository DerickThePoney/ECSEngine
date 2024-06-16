#include "stdafx.h"

#include "WorldDeclaration.h"

#include "ApparenceModule.h"
#include "CameraMoverModule.h"
#include "ECSCore/EntityWorld.h"
#include "ECSCore/WorldIds.h"
#include "ECSCore/WorldManager.h"
#include "ECSGameplay_Common/BoxColliderModule.h"
#include "ECSGameplay_Common/RigidbodyModule.h"
#include "ECSGameplay_Specific/BuildingGridOccupancyModule.h"
#include "ECSGameplay_Specific/ColonyModule.h"
#include "ECSGameplay_Specific/EnergyConsumerModule.h"
#include "ECSGameplay_Specific/EnergyProducerModule.h"
#include "ECSGameplay_Specific/LinkToStorageModule.h"
#include "ECSGameplay_Specific/RecipeProductionModule.h"
#include "ECSGameplay_Specific/ResourceStorageModule.h"
#include "ECSGameplay_Specific/StorageSlotModule.h"
#include "OrientationModule.h"
#include "PositionModule.h"
#include "brigand/algorithms/for_each.hpp"
#include "brigand/sequences/list.hpp"

namespace ECSEngine
{
using StandardControllers =
      brigand::list<ECSEngine::PositionModule, ECSEngine::OrientationModule, ECSEngine::ApparenceModule, ECSEngine::RigidbodyModule, ECSEngine::BoxColliderModule>;

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

using CameraControllers = brigand::list<ECSEngine::PositionModule, ECSEngine::OrientationModule, ECSEngine::CameraMoverModule>;

struct f
{
    template<typename U>
    void operator()(brigand::type_<U>)
    {
        world->AddController<U>();
    }

    EntityWorld* world;
};

void CreateWorlds()
{
    {
        EntityWorld* world = new EntityWorld(EEntityWorlds::STANDARD);
        auto r = brigand::for_each<StandardControllers>(f{ world });
        WorldManager::Instance().AddEntityWorldStealOwnership(EEntityWorlds::STANDARD, world);
    }

    {
        EntityWorld* world = new EntityWorld(EEntityWorlds::COLONY);
        auto r = brigand::for_each<ColonyControllers>(f{ world });
        WorldManager::Instance().AddEntityWorldStealOwnership(EEntityWorlds::COLONY, world);
    }

    {
        EntityWorld* world = new EntityWorld(EEntityWorlds::CAMERA);
        auto r = brigand::for_each<CameraControllers>(f{ world });
        WorldManager::Instance().AddEntityWorldStealOwnership(EEntityWorlds::CAMERA, world);
    }

    {
        EntityWorld* world = new EntityWorld(EEntityWorlds::RESOURCE_PROD);
        auto r = brigand::for_each<ResourceProducerControllers>(f{ world });
        WorldManager::Instance().AddEntityWorldStealOwnership(EEntityWorlds::RESOURCE_PROD, world);
    }

    {
        EntityWorld* world = new EntityWorld(EEntityWorlds::BUILDINGS);
        auto r = brigand::for_each<BuildingControllers>(f{ world });
        WorldManager::Instance().AddEntityWorldStealOwnership(EEntityWorlds::BUILDINGS, world);
    }
}
} // namespace ECSEngine
