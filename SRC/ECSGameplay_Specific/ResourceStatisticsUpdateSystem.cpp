#include "stdafx.h"

#include "ResourceStatisticsUpdateSystem.h"

#include "Common/TimeManager.h"
#include "ECSCore/ModuleAccessor.h"
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
    RegisterDepency<ResourceStorageModule>(Worlds::COLONY);
    RegisterDepency<ResourceStorageModule>(Worlds::RESOURCE_PROD);
    RegisterDepency<ResourceStorageModule>(Worlds::PEONS);
}

void ResourceStatisticsUpdateSystem::VirtualUpdate()
{
    ModuleSystem::VirtualUpdate();

    ModuleAccessor<ResourceStorageModule> colonyStorageAccessor(Worlds::COLONY);
    ModuleAccessor<ResourceStorageModule> peonsStorageAccessor(Worlds::PEONS);
    ModuleAccessor<ResourceStorageModule> producerStorageAccessor(Worlds::RESOURCE_PROD);

    UpdateStorageStats(colonyStorageAccessor);
    UpdateStorageStats(producerStorageAccessor);
    UpdateStorageStats(peonsStorageAccessor);
}

} // namespace ECSEngine