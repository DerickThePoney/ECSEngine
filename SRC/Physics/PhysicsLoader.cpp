#include "stdafx.h"

#include "PhysicsLoader.h"

#include "PhysicsAPI.h"

namespace ECSEngine
{

bool PhysicsLoader::VirtualInitialise()
{
    ILoader::VirtualInitialise();

    Physics::InitializePhysics(FConfigurationFilename);
    return true;
}

void PhysicsLoader::VirtualShutdown()
{
    ILoader::VirtualShutdown();

    Physics::ShutdownPhysics();
}

} // namespace ECSEngine

CEREAL_REGISTER_POLYMORPHIC_RELATION(ECSEngine::ILoader, ECSEngine::PhysicsLoader);