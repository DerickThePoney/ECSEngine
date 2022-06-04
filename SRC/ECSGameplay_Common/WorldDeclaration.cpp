#include "stdafx.h"

#include "WorldDeclaration.h"

#include "ApparenceModule.h"
#include "CameraMoverModule.h"
#include "ECSCore/EntityWorld.h"
#include "ECSCore/ModuleTemplate.h"
#include "ECSCore/WorldIds.h"
#include "ECSCore/WorldManager.h"
#include "ECSGameplay_Specific/BuildingGridOccupancyModule.h"
#include "ECSGameplay_Specific/ColonyModule.h"
#include "ECSGameplay_Specific/ColonyPeonsManagementModule.h"
#include "ECSGameplay_Specific/ColonyTraitsModule.h"
#include "ECSGameplay_Specific/EnergyConsumerModule.h"
#include "ECSGameplay_Specific/EnergyProducerModule.h"
#include "ECSGameplay_Specific/HousingPlaceModule.h"
#include "ECSGameplay_Specific/LinkToHousingPlaceModule.h"
#include "ECSGameplay_Specific/LinkToStorageModule.h"
#include "ECSGameplay_Specific/LinkToWorkPlaceModule.h"
#include "ECSGameplay_Specific/PeonFeedingTimeModule.h"
#include "ECSGameplay_Specific/PeonSpawnModule.h"
#include "ECSGameplay_Specific/RecipeProductionModule.h"
#include "ECSGameplay_Specific/ResourceHarvesterModule.h"
#include "ECSGameplay_Specific/ResourceStorageModule.h"
#include "ECSGameplay_Specific/StorageSlotModule.h"
#include "ECSGameplay_Specific/WorkPlaceModule.h"
#include "EntityLinksModules.h"
#include "MovementModule.h"
#include "OrientationModule.h"
#include "PositionModule.h"
#include "brigand/algorithms/for_each.hpp"
#include "brigand/sequences/list.hpp"

namespace ECSEngine
{
using StandardControllers = brigand::list<ECSEngine::PositionModule, ECSEngine::OrientationModule, ECSEngine::ApparenceModule, ECSEngine::MovementModule>;

using PeonsControllers = brigand::list<ECSEngine::PositionModule,
      ECSEngine::OrientationModule,
      ECSEngine::ApparenceModule,
      ECSEngine::MovementModule,
      ECSEngine::ResourceStorageModule,
      ECSEngine::LinkToOwnerModule,
      ECSEngine::ResourceHarvesterModule,
      ECSEngine::LinkToHousingPlaceModule,
      ECSEngine::LinkToWorkPlaceModule>;

using ResourceProducerControllers = brigand::list<ECSEngine::PositionModule, ECSEngine::OrientationModule, ECSEngine::ApparenceModule, ECSEngine::ResourceStorageModule>;

using ColonyControllers = brigand::list<ECSEngine::PositionModule,
      ECSEngine::ColonyModule,
      ECSEngine::ResourceStorageModule,
      ECSEngine::ColonyPeonsManagementModule,
      ECSEngine::PeonSpawnModule,
      ECSEngine::PeonFeedingTimeModule,
      ECSEngine::ColonyTraitsModule>;

using BuildingControllers = brigand::list<ECSEngine::PositionModule,
      ECSEngine::OrientationModule,
      ECSEngine::ApparenceModule,
      ECSEngine::ResourceStorageModule,
      ECSEngine::RecipeProductionModule,
      ECSEngine::HousingPlaceModule,
      ECSEngine::BuildingGridOccupancyModule,
      ECSEngine::WorkPlaceModule,
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
        EntityWorld* world = new EntityWorld(EEntityWorlds::PEONS);
        auto r = brigand::for_each<PeonsControllers>(f{ world });
        WorldManager::Instance().AddEntityWorldStealOwnership(EEntityWorlds::PEONS, world);
    }

    {
        EntityWorld* world = new EntityWorld(EEntityWorlds::BUILDINGS);
        auto r = brigand::for_each<BuildingControllers>(f{ world });
        WorldManager::Instance().AddEntityWorldStealOwnership(EEntityWorlds::BUILDINGS, world);
    }
}

namespace ModuleTemplates
{
std::unordered_map<u32, ModuleTemplate* (*)()> FModuleTemplateFactories;
std::map<u32, std::string> FModuleList;
void InitModuleTemplateFactories()
{
#define DECLARE_MODULE_AND_TEMPLATE(NAME, TEMPLATE)                                                                                                                                \
    {                                                                                                                                                                              \
        FModuleTemplateFactories[ModuleTraits<NAME>::GetModuleId()] = &TEMPLATE::CreateTemplate;                                                                                   \
    }
#include "ECSCore/ModuleList.inl"
#undef DECLARE_MODULE_AND_TEMPLATE
}

void DestroyModyleTemplateFactories()
{
}

} // namespace ModuleTemplates
} // namespace ECSEngine
