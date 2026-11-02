#include "stdafx.h"

#include "PlanetGravitySystem.h"

#include "ECSCore/ModuleAccessor.h"
#include "ECSCore/WorldIds.h"
#include "ECSGameplay_Common/PositionModule.h"
#include "ECSGameplay_Common/RigidbodyModule.h"
#include "Physics/PhysicsAPI.h"
#include "PlanetGravityModule.h"
#include "RocketManagerModule.h"

namespace ECSEngine
{
void PlanetGravitySystem::VirtualInit()
{
    ModuleSystem::VirtualInit();

    RegisterDependency<RigidbodyModule>(EEntityWorlds::STANDARD);
    RegisterDependency<PositionModule>(EEntityWorlds::STANDARD);
    RegisterDependency<PlanetGravityModule>(EEntityWorlds::STANDARD);
    RegisterDependency<RocketManagerModule>(EEntityWorlds::STANDARD);
}

void PlanetGravitySystem::VirtualUpdate()
{
    ModuleSystem::VirtualUpdate();

    ModuleAccessor<RocketManagerModule> rocketManagerAccessor(EEntityWorlds::STANDARD);
    ModuleAccessor<RigidbodyModule> rigidbodyAccessor(EEntityWorlds::STANDARD);
    ModuleAccessor<PositionModule> positionAccessor(EEntityWorlds::STANDARD);
    ModuleAccessor<PlanetGravityModule> planetGravityAccessor(EEntityWorlds::STANDARD);

    foreachitem(rocket, rocketManagerAccessor)
    {
        const EntityId& rocketBodyId = rocket.BodyId();
        const PositionModule* const bodyPositionModule = positionAccessor[rocketBodyId];
        AssertRelease(bodyPositionModule != nullptr);

        vec3 gravityAcceleration{ 0.f };

        foreachitemconst(planetGravity, planetGravityAccessor)
        {
            const EntityId& planetId = planetGravity.UnitId();
            const PositionModule* const planetPositionModule = positionAccessor[planetId];
            AssertRelease(planetPositionModule != nullptr);

            // compute gravity
            vec3 r = bodyPositionModule->GetPosition3D() - planetPositionModule->GetPosition3D();
            const float lr = Length(r);
            const float lr_3 = lr * lr * lr;
            gravityAcceleration -= planetGravity.PlanetGravityConstant() * r / lr_3;
        }

        // Hackos
        rocket.SetLastTurnGravity(gravityAcceleration);

        // TODO: make sure to be able to loop over the whole rocket parts
        const RigidbodyModule* const bodyRB = rigidbodyAccessor[rocketBodyId];
        AssertRelease(bodyRB != nullptr);
        const RigidbodyModule* const nozzleRB = rigidbodyAccessor[rocket.NozzleId()];
        AssertRelease(nozzleRB != nullptr);

        Physics::AddForceToBody(bodyRB->BodyHandle(), gravityAcceleration, true);
        Physics::AddForceToBody(nozzleRB->BodyHandle(), gravityAcceleration, true);
    }
}
} // namespace ECSEngine
