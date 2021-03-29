#include "stdafx.h"

#include "PeonSpawnSystem.h"

#include "ColonyPeonsManagementModule.h"
#include "ECSCore/EntityFactory.h"
#include "ECSCore/ModuleAccessor.h"
#include "ECSCore/ModuleParameters.h"
#include "ECSGameplay_Common/PositionModule.h"
#include "GameplayRulesManager.h"
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

    foreachitem(spawnModule, colonySpawnModuleAccessor)
    {
        if (!GameplayRulesManager::Instance().FPeonSpawningRulesManager.AutoSpawn() && !spawnModule.RequestedPeonCreation())
            continue;

        SpawnPeonOrder order = spawnModule.PopPeonSpawnOrder();

        OrderExecutor<SpawnPeonOrder> executor(std::move(order));
        executor.ExecuteOrder();
    }
}

} // namespace ECSEngine
