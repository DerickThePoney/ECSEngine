#include "stdafx.h"

#include "EnergySystem.h"

#include "ECSCore/ModuleAccessor.h"
#include "ECSCore/WorldIds.h"
#include "EnergyConsumerModule.h"
#include "EnergyProducerModule.h"

namespace ECSEngine
{

EnergySystem::EnergySystem()
    : ModuleSystem()
    , Singleton<EnergySystem>()
{
    RegisterDepency<EnergyProducerModule>(EEntityWorlds::BUILDINGS);
    RegisterDepency<EnergyConsumerModule>(EEntityWorlds::BUILDINGS);
}

float EnergySystem::ConsumedToProducedEnergyRatio() const
{
    return FConsumedToProducedEnergyRatio;
}

i32 EnergySystem::TotalEnergy() const
{
    return (i32)FProducedEnergy - (i32)FConsumedEnergy;
}

void EnergySystem::Finalize()
{
    ModuleSystem::Destroy();
}

void EnergySystem::Delete()
{
    Singleton<EnergySystem>::Destroy();
}

void EnergySystem::VirtualUpdate()
{
    ModuleSystem::VirtualUpdate();

    {
        ModuleAccessor<EnergyProducerModule> accessor(EEntityWorlds::BUILDINGS);
        FProducedEnergy = 0;
        foreachitemconst(energyModule, accessor) { FProducedEnergy += energyModule.ProducedEnergy(); }
    }

    {
        ModuleAccessor<EnergyConsumerModule> accessor(EEntityWorlds::BUILDINGS);
        FConsumedEnergy = 0;
        foreachitemconst(energyModule, accessor) { FConsumedEnergy += energyModule.ConsumedEnergy(); }
    }

    FConsumedToProducedEnergyRatio = (float)FConsumedEnergy / (float)FProducedEnergy;
}
} // namespace ECSEngine