#include "stdafx.h"

#include "WorldDeclaration.h"

#include "ApparenceModule.h"
#include "CameraMoverModule.h"
#include "ECSCore/EntityWorld.h"
#include "ECSCore/ModuleTemplate.h"
#include "ECSCore/WorldManager.h"
#include "ECSGameplay_Specific/ColonyModule.h"
#include "MovementModule.h"
#include "OrientationModule.h"
#include "PositionModule.h"

#include <standalone/brigand.hpp>

namespace ECSEngine
{
using StandardControllers = brigand::list<ECSEngine::PositionModule, ECSEngine::OrientationModule, ECSEngine::ApparenceModule, ECSEngine::MovementModule>;

using ColonyControllers = brigand::list<ECSEngine::PositionModule, ECSEngine::ColonyModule>;

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
        EntityWorld* world = new EntityWorld(Worlds::STANDARD);
        auto r = brigand::for_each<StandardControllers>(f{ world });
        WorldManager::Instance().AddEntityWorldStealOwnership(Worlds::STANDARD, world);
    }

    {
        EntityWorld* world = new EntityWorld(Worlds::COLONY);
        auto r = brigand::for_each<ColonyControllers>(f{ world });
        WorldManager::Instance().AddEntityWorldStealOwnership(Worlds::COLONY, world);
    }

    {
        EntityWorld* world = new EntityWorld(Worlds::CAMERA);
        auto r = brigand::for_each<CameraControllers>(f{ world });
        WorldManager::Instance().AddEntityWorldStealOwnership(Worlds::CAMERA, world);
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