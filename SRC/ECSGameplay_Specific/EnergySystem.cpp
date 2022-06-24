#include "stdafx.h"

#include "EnergySystem.h"

#include "Common/SavingSystemImplementation.h"
#include "ECSCore/ModuleAccessor.h"
#include "ECSCore/WorldIds.h"
#include "EnergyConsumerModule.h"
#include "EnergyProducerModule.h"
#include "GameplayConstants.h"

namespace ECSEngine
{
IMPLEMENT_SAVELOAD_ABILITIES(EnergySystem);
template<typename Chunk, bool isWriting>
void EnergySystem::SaveLoad(Chunk& parChunk)
{
    parChunk& FConsumedEnergy;
    parChunk& FProducedEnergy;
    parChunk& FConsumedToProducedEnergyRatio;
    parChunk& FEnergyEfficiency;
}

EnergySystem::EnergySystem()
    : ModuleSystem()
    , Singleton<EnergySystem>()
{
    RegisterDepency<EnergyProducerModule>(EEntityWorlds::BUILDINGS);
    RegisterDepency<EnergyConsumerModule>(EEntityWorlds::BUILDINGS);
}

i32 EnergySystem::TotalAvailableEnergy() const
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

    if (FProducedEnergy == 0 && FConsumedEnergy > 0)
        FConsumedToProducedEnergyRatio = 0.f;
    else if (FProducedEnergy >= FConsumedEnergy)
        FConsumedToProducedEnergyRatio = 1.f;
    else
        FConsumedToProducedEnergyRatio = (float)FProducedEnergy / (float)FConsumedEnergy;

    FEnergyEfficiency = GameplayConstants::Energy::EnergyEfficiency.GetEfficiency(FConsumedToProducedEnergyRatio);
}
} // namespace ECSEngine