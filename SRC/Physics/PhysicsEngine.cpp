#include "stdafx.h"

#include "PhysicsEngine.h"

#include "PhysicsBodyHandle.h"

namespace ECSEngine
{
PhysicsEngine::PhysicsEngine()
    : Singleton()
{
}

PhysicsEngine::~PhysicsEngine()
{
}

void PhysicsEngine::Initialize(const PhysicsEngineConfiguration& PhysicsConfig)
{
    SetConfig(PhysicsConfig);
}

void PhysicsEngine::Cleanup()
{
}

const PhysicsBodyHandle PhysicsEngine::CreateNewBody()
{
    return PhysicsBodyHandle();
}

} // namespace ECSEngine
