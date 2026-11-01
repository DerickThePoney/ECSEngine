#include "stdafx.h"

#include "WorldDeclaration.h"

#include "ApparenceModule.h"
#include "BoxColliderModule.h"
#include "CameraMoverModule.h"
#include "ECSCore/EntityWorld.h"
#include "ECSCore/WorldIds.h"
#include "ECSCore/WorldManager.h"
#include "OrientationModule.h"
#include "PositionModule.h"
#include "RigidbodyModule.h"
#include "brigand/algorithms/for_each.hpp"
#include "brigand/sequences/list.hpp"

namespace ECSEngine
{
using StandardControllers = brigand::list<ECSEngine::PositionModule, ECSEngine::OrientationModule, ECSEngine::ApparenceModule, ECSEngine::RigidbodyModule, ECSEngine::BoxColliderModule>;

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
        EntityWorld* world = new EntityWorld(EEntityWorlds::CAMERA);
        auto r = brigand::for_each<CameraControllers>(f{ world });
        WorldManager::Instance().AddEntityWorldStealOwnership(EEntityWorlds::CAMERA, world);
    }
}
} // namespace ECSEngine
