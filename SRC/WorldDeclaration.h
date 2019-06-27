#pragma once
#include "EntityWorld.h"
#include "ModuleId.h"
#include "PositionModule.h"
#include "Types.h"

#include <standalone/brigand.hpp>

namespace ECSEngine
{

using Controllers = brigand::list<ECSEngine::PositionModule>;

struct f
{
    template<typename U>
    void operator()(brigand::type_<U>)
    {
        world->AddController<U>();
        std::cout << "Adding controller of type: " << typeid(U).name() << std::endl;
    }

    EntityWorld* world;
};

EntityWorld CreateWorld()
{
    EntityWorld world;
    auto r = brigand::for_each<Controllers>(f{ &world });
    return world;
}
} // namespace ECSEngine
