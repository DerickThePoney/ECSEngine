#include "stdafx.h"

#include "PhysicsUpdateSystem.h"

#include "Physics/PhysicsAPI.h"

namespace ECSEngine
{

PhysicsUpdateSystem::PhysicsUpdateSystem()
    : parent_type()
{
}

PhysicsUpdateSystem::~PhysicsUpdateSystem()
{
}

void PhysicsUpdateSystem::VirtualUpdate()
{
    parent_type::VirtualUpdate();

    Physics::UpdatePhysics();
}

} // namespace ECSEngine
