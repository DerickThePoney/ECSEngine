#include "stdafx.h"

#include "DebrisLifetimeSystem.h"

#include "Common/TimeManager.h"
#include "ECSCore/ModuleAccessor.h"
#include "ECSCore/WorldIds.h"
#include "ECSGameplay_Common/RigidbodyModule.h"
#include "RocketManagerModule.h"
#include "TimeWarpManager.h"

namespace ECSEngine
{
DebrisLifetimeSystem::DebrisLifetimeSystem()
{
    RegisterDependency<RocketManagerModule>(EEntityWorlds::STANDARD);
    RegisterDependency<RigidbodyModule>(EEntityWorlds::STANDARD);
}

void DebrisLifetimeSystem::VirtualUpdate()
{
    ModuleSystem::VirtualUpdate();

    ModuleAccessor<RocketManagerModule> rocketManagerAccessor(EEntityWorlds::STANDARD);
    ModuleAccessor<RigidbodyModule> rigidbodyAccessor(EEntityWorlds::STANDARD);

    const bool bIsWarping = TimeWarpManager::Instance().CurrentTimeWarpMultiplier() > 1;
    const float DT = TimeManager::GameplayDeltaTime();
    foreachitem(rocket, rocketManagerAccessor)
    {
        if (bIsWarping)
        {
            rocket.KillAllDebris();
            continue;
        }

        // lifetime cull
        rocket.IncrementDebrisLifetime(DT);
        reverseforrange(i, 0, rocket.DebrisCount())
        {
            if (rocket.LifetimeForDebrisIndex(i) > rocket.DebrisMaxLifetime())
                rocket.KillDebris(i);
        }

        // distance cull
        vec3 rocketPos;
        if (!rocket.ComputeActiveCenterOfMass(rigidbodyAccessor, rocketPos))
            continue;

        reverseforrange(i, 0, rocket.DebrisCount())
        {
            vec3 debrisCom;
            if (!rocket.ComputeCenterOfMassInDebrisIndex(rigidbodyAccessor, i, debrisCom))
                continue;

            float CullRadius = rocket.DebrisCullRadius();
            if (LengthSq(debrisCom - rocketPos) > (CullRadius * CullRadius))
                rocket.KillDebris(i);
        }
    }
}
} // namespace ECSEngine
