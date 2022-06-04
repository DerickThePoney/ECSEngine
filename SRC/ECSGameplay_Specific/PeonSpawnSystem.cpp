#include "stdafx.h"

#include "PeonSpawnSystem.h"

#include "ColonyPeonsManagementModule.h"
#include "ECSCore/EntityFactory.h"
#include "ECSCore/ModuleAccessor.h"
#include "ECSCore/ModuleParameters.h"
#include "ECSCore/WorldIds.h"
#include "ECSGameplay_Common/PositionModule.h"
#include "GameplayRulesManager.h"
#include "PeonSpawnModule.h"
#include "ResourceStorageModule.h"

namespace ECSEngine
{

PeonSpawnSystem::PeonSpawnSystem()
    : ModuleSystem()
{
    RegisterDepency<PeonSpawnModule>(EEntityWorlds::COLONY);
    RegisterDepency<ColonyPeonsManagementModule>(EEntityWorlds::COLONY);
    RegisterDepency<ResourceStorageModule>(EEntityWorlds::COLONY);
    RegisterDepency<PositionModule>(EEntityWorlds::COLONY);
}

PeonSpawnSystem::~PeonSpawnSystem()
{
}

void PeonSpawnSystem::VirtualUpdate()
{
    ModuleSystem::VirtualUpdate();

    ModuleAccessor<PeonSpawnModule> colonySpawnModuleAccessor(EEntityWorlds::COLONY);
    ModuleAccessor<ColonyPeonsManagementModule> colonyPeonsAccessor(EEntityWorlds::COLONY);
    ModuleAccessor<ResourceStorageModule> colonyResourceStorageAccessor(EEntityWorlds::COLONY);
    ModuleAccessor<PositionModule> colonyPositionAccessor(EEntityWorlds::COLONY);

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
