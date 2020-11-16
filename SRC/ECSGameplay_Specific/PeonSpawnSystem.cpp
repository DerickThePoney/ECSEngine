#include "stdafx.h"

#include "PeonSpawnSystem.h"

#include "ColonyPeonsManagementModule.h"
#include "ECSCore/EntityFactory.h"
#include "ECSCore/ModuleAccessor.h"
#include "ECSCore/ModuleParameters.h"
#include "ECSGameplay_Common/PositionModule.h"
#include "PeonSpawnModule.h"
#include "ResourceStorageModule.h"

namespace ECSEngine
{

PeonSpawnSystem::PeonSpawnSystem()
    : ModuleSystem()
{
    RegisterDepency<PeonSpawnModule>(Worlds::COLONY);
    RegisterDepency<ColonyPeonsManagementModule>(Worlds::COLONY);
    RegisterDepency<ResourceStorageModule>(Worlds::COLONY);
    RegisterDepency<PositionModule>(Worlds::COLONY);
}

PeonSpawnSystem::~PeonSpawnSystem()
{
}

void PeonSpawnSystem::VirtualUpdate()
{
    ModuleSystem::VirtualUpdate();

    ModuleAccessor<PeonSpawnModule> colonySpawnModuleAccessor(Worlds::COLONY);
    ModuleAccessor<ColonyPeonsManagementModule> colonyPeonsAccessor(Worlds::COLONY);
    ModuleAccessor<ResourceStorageModule> colonyResourceStorageAccessor(Worlds::COLONY);
    ModuleAccessor<PositionModule> colonyPositionAccessor(Worlds::COLONY);

    foreachitemconst(spawnModule, colonySpawnModuleAccessor)
    {
        ColonyPeonsManagementModule* colonyPeons = colonyPeonsAccessor[spawnModule.UnitId()];
        AssertRelease(colonyPeons != nullptr);
        ResourceStorageModule* colonyStorage = colonyResourceStorageAccessor[spawnModule.UnitId()];
        AssertRelease(colonyStorage != nullptr);

        const u32 currentPeons = colonyPeons->PeonsInColony();
        const u32 resourceInStorage = colonyStorage->GetResourceQuantity(spawnModule.ResourceToPay());
        const u32 costForNextSpawn = spawnModule.CostForNextSpawn(currentPeons);

        if (costForNextSpawn <= resourceInStorage)
        {
            const PositionModule* colonyPos = colonyPositionAccessor[spawnModule.UnitId()];
            AssertRelease(colonyPos != nullptr);

            ModuleParameters::ParameterContainer container;
            container.Set<ModuleParameters::Position>(colonyPos->GetPosition3D());
            container.Set<ModuleParameters::OwnerId>(spawnModule.UnitId());

            EntityId newPeon = EntityFactory::CreateEntity(spawnModule.Template<PeonSpawnModuleTemplate>()->PeonTemplate(), container);
            colonyPeons->AddNewPeon(newPeon);

            const u32 payed = colonyStorage->RemoveResource(spawnModule.ResourceToPay(), costForNextSpawn);
            AlwaysCheckedAssert(payed == costForNextSpawn);
        }
    }
}

} // namespace ECSEngine