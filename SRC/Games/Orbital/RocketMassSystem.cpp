#include "stdafx.h"

#include "RocketMassSystem.h"

#include "ECSCore/ModuleAccessor.h"
#include "ECSCore/WorldIds.h"
#include "ECSGameplay_Common/RigidbodyModule.h"
#include "FuelTankModule.h"
#include "Physics/PhysicsAPI.h"

namespace ECSEngine
{
RocketMassSystem::RocketMassSystem()
{
    RegisterDependency<FuelTankModule>(EEntityWorlds::STANDARD);
    RegisterDependency<RigidbodyModule>(EEntityWorlds::STANDARD);
}

void RocketMassSystem::VirtualUpdate()
{
    ModuleSystem::VirtualUpdate();

    ModuleAccessor<FuelTankModule> bodyTankAccessor(EEntityWorlds::STANDARD);
    ModuleAccessor<RigidbodyModule> rigidbodyAccessor(EEntityWorlds::STANDARD);

    foreachitem(fuelTank, bodyTankAccessor)
    {
        RigidbodyModule* tankBody = rigidbodyAccessor[fuelTank.UnitId()];
        AssertRelease(tankBody != nullptr);

        if (!fuelTank.DryMassSet())
        {
            float DryMass = 0.f;
            if (Physics::GetBodyMass(tankBody->BodyHandle(), DryMass))
            {
                fuelTank.SetDryMass(DryMass);
            }
        }

        if (!fuelTank.DryMassSet())
            continue;

        Physics::SetBodyMass(tankBody->BodyHandle(), fuelTank.DryMass() + fuelTank.CurrentRemainingFuelMass());
    }
}
} // namespace ECSEngine