#include "stdafx.h"

#include "ResourceStatisticsUpdateSystem.h"

#include "Common/TimeManager.h"
#include "ECSCore/ModuleAccessor.h"
#include "ECSCore/WorldIds.h"
#include "ResourceStorageModule.h"

namespace ECSEngine
{

namespace
{
void UpdateStorageStats(ModuleAccessor<ResourceStorageModule>& parAccessor)
{
    const float now = TimeManager::FrameStartTime();
    foreachitem(storage, parAccessor)
    {
        ResourcesStatistics& stats = storage.Statistics();
        stats.UpdateStatistics(now);
    }
}
} // namespace

ResourceStatisticsUpdateSystem::ResourceStatisticsUpdateSystem()
    : ModuleSystem()
{
    RegisterDepency<ResourceStorageModule>(EEntityWorlds::COLONY);
    RegisterDepency<ResourceStorageModule>(EEntityWorlds::RESOURCE_PROD);
    RegisterDepency<ResourceStorageModule>(EEntityWorlds::PEONS);
}

void ResourceStatisticsUpdateSystem::VirtualUpdate()
{
    ModuleSystem::VirtualUpdate();

    ModuleAccessor<ResourceStorageModule> colonyStorageAccessor(EEntityWorlds::COLONY);
    ModuleAccessor<ResourceStorageModule> peonsStorageAccessor(EEntityWorlds::PEONS);
    ModuleAccessor<ResourceStorageModule> producerStorageAccessor(EEntityWorlds::RESOURCE_PROD);

    UpdateStorageStats(colonyStorageAccessor);
    UpdateStorageStats(producerStorageAccessor);
    UpdateStorageStats(peonsStorageAccessor);
}

} // namespace ECSEngine
