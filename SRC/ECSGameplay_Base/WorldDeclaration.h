#pragma once

#include "ApparenceModule.h"
#include "Common/Types.h"
#include "ECSBase/EntityWorld.h"
#include "ECSBase/ModuleId.h"

#include <standalone/brigand.hpp>

namespace ECSEngine
{

using Controllers = brigand::list</*ECSEngine::PositionModule, ECSEngine::OrientationModule*/ ECSEngine::ApparenceModule>;

struct f
{
    template<typename U>
    void operator()(brigand::type_<U>)
    {
        world->AddController<U>();
    }

    EntityWorld* world;
};

void CreateWorld(EntityWorld& world)
{
    auto r = brigand::for_each<Controllers>(f{ &world });
}
} // namespace ECSEngine
