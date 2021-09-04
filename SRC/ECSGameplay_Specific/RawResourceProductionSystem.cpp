#include "stdafx.h"

#include "RawResourceProductionSystem.h"

#include "Common/TimeManager.h"
#include "ECSCore/ModuleAccessor.h"
#include "RawResourceProductionModule.h"
#include "ResourceStorageModule.h"

namespace ECSEngine
{

RawResourceProductionSystem::RawResourceProductionSystem()
    : ModuleSystem()
{
    RegisterDepency<RawResourceProductionModule>(Worlds::RESOURCE_PROD);
    RegisterDepency<ResourceStorageModule>(Worlds::RESOURCE_PROD);
}

RawResourceProductionSystem::~RawResourceProductionSystem()
{
}

void RawResourceProductionSystem::VirtualUpdate()
{
    ModuleSystem::VirtualUpdate();

    ModuleAccessor<RawResourceProductionModule> resourceProductionAccessor(Worlds::RESOURCE_PROD);
    ModuleAccessor<ResourceStorageModule> resourceStorageAccesor(Worlds::RESOURCE_PROD);

    foreachitem(producer, resourceProductionAccessor)
    {
        ResourceStorageModule* producerStorage = resourceStorageAccesor[producer.UnitId()];
        AssertRelease(producerStorage != nullptr);

        const u32 remainingStorageSpace = producerStorage->GetRemainingStorageSpace();
        u32 producedResources = 0;
        const float dt = TimeManager::GameplayDeltaTime();
        MemoryView<RawResourceProductionModule::ProducedRawResourceTiming> resourceTimings = producer.ProducedResourcesTimings();
        foreachitem(resource, resourceTimings)
        {
            if (producedResources >= remainingStorageSpace)
            {
                producer.ResetTimingForResource(resource.first);
                break;
            }
            resource.second -= dt;
            if (resource.second <= 0.f)
            {
                const u32 addedResource = producerStorage->AddResource(resource.first, 1);
                producedResources += addedResource;
                producer.ResetTimingForResource(resource.first);
            }
        }
    }
}

} // namespace ECSEngine
