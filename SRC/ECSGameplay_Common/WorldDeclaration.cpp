#include "stdafx.h"

#include "WorldDeclaration.h"

#include "ApparenceModule.h"
#include "ECSCore/EntityWorld.h"
#include "ECSCore/ModuleTemplate.h"
#include "ECSCore/WorldManager.h"
#include "OrientationModule.h"
#include "PositionModule.h"

#include <standalone/brigand.hpp>

namespace ECSEngine
{
using Controllers = brigand::list<ECSEngine::PositionModule, ECSEngine::OrientationModule, ECSEngine::ApparenceModule>;

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
    EntityWorld* world = new EntityWorld(Worlds::STANDARD);
    auto r = brigand::for_each<Controllers>(f{ world });
    WorldManager::Instance().AddEntityWorldStealOwnership(Worlds::STANDARD, world);
}

namespace ModuleTemplates
{
std::unordered_map<u32, ModuleTemplate* (*)()> FModuleTemplateFactories;
std::map<u32, std::string> FModuleList;
void InitModuleTemplateFactories()
{
#define DECLARE_MODULE_AND_TEMPLATE(NAME, TEMPLATE)                                                                                                                                \
    {                                                                                                                                                                              \
        FModuleTemplateFactories[TEMPLATE::GetId()] = &TEMPLATE::CreateTemplate##TEMPLATE;                                                                                         \
    }
#include "ECSCore/ModuleList.inl"
#undef DECLARE_MODULE_AND_TEMPLATE
}

void DestroyModyleTemplateFactories()
{
}

} // namespace ModuleTemplates
} // namespace ECSEngine