#include "stdafx.h"

#include "PhysicsFloatingOriginSystem.h"

#include "ECSCore/ModuleAccessor.h"
#include "ECSCore/WorldIds.h"
#include "ECSGameplay_Common/PhysicsFloatingOriginModule.h"
#include "ECSGameplay_Common/RigidbodyModule.h"
#include "Physics/PhysicsAPI.h"
#include "Physics/PhysicsEngineConfiguration.h"
#include "RenderingCore/GFXRepresentationManager.h"
#include "RocketManagerModule.h"

namespace ECSEngine
{

PhysicsFloatingOriginSystem::PhysicsFloatingOriginSystem()
{
    RegisterDependency<RocketManagerModule>(EEntityWorlds::STANDARD);
    RegisterDependency<PhysicsFloatingOriginModule>(EEntityWorlds::STANDARD);
    RegisterDependency<RigidbodyModule>(EEntityWorlds::STANDARD);
}

void PhysicsFloatingOriginSystem::VirtualUpdate()
{
    ModuleSystem::VirtualUpdate();

    if (!Physics::GetConfig().FbEnableOriginShift)
        return;

    ModuleAccessor<RocketManagerModule> rocketManagerAccessor(EEntityWorlds::STANDARD);
    ModuleAccessor<RigidbodyModule> rigidbodyAccessor(EEntityWorlds::STANDARD);
    ModuleAccessor<PhysicsFloatingOriginModule> floatingOriginAccessor(EEntityWorlds::STANDARD);

    AssertReleaseMsg(floatingOriginAccessor.size() <= 1, "There can be only one PhysicsFloatingOriginModule in the world !");

    foreachitem(floatingOrigin, floatingOriginAccessor)
    {
        const RocketManagerModule* rocketManager = rocketManagerAccessor[floatingOrigin.UnitId()];
        AssertRelease(rocketManager != nullptr);

        vec3 rocketCOM;
        if (!rocketManager->ComputeCenterOfMass(rigidbodyAccessor, rocketCOM))
            continue;

        if (Length(rocketCOM) <= floatingOrigin.BubbleRadius())
            continue;

        const vec3 Shift = Invert(rocketCOM);
        if (Physics::ShiftWorld(Shift))
        {
            floatingOrigin.SetOrigin(floatingOrigin.Origin() + dvec3(rocketCOM.x, rocketCOM.y, rocketCOM.z));
            Rendering::GFXRepresentationManager::Instance().ShiftRepresentations(Shift);
        }
    }
}

} // namespace ECSEngine