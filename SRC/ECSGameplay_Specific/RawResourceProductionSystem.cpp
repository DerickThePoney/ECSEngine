#include "stdafx.h"

#include "RawResourceProductionSystem.h"

#include "Common/TimeManager.h"
#include "ECSCore/ModuleAccessor.h"
#include "RawResourceProductionModule.h"
#include "RecipeProductionModule.h"
#include "ResourceStorageModule.h"

namespace ECSEngine
{

RawResourceProductionSystem::RawResourceProductionSystem()
    : ModuleSystem()
{
    RegisterDepency<RawResourceProductionModule>(Worlds::BUILDINGS);
    RegisterDepency<RecipeProductionModule>(Worlds::BUILDINGS);
    RegisterDepency<ResourceStorageModule>(Worlds::BUILDINGS);
}

RawResourceProductionSystem::~RawResourceProductionSystem()
{
}

void RawResourceProductionSystem::VirtualUpdate()
{
    ModuleSystem::VirtualUpdate();

    ModuleAccessor<RawResourceProductionModule> resourceProductionAccessor(Worlds::BUILDINGS);
    ModuleAccessor<ResourceStorageModule> resourceStorageAccesor(Worlds::BUILDINGS);

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

    ModuleAccessor<RecipeProductionModule> recipeProductionAccessor(Worlds::BUILDINGS);
    foreachitem(producer, recipeProductionAccessor)
    {
        const ProductionRecipe* recipe = producer.GetProductionRecipe();
        AlwaysCheckedAssert(recipe != nullptr);
        if (recipe == nullptr)
            continue;

        ResourceStorageModule* producerStorage = resourceStorageAccesor[producer.UnitId()];
        AssertRelease(producerStorage != nullptr);

        switch (producer.State())
        {
        case RecipeProductionState::IDLE:
        {
            MemoryView<const RecipeComponent> inputComponents = recipe->InputComponents();
            MemoryView<const RecipeComponent> outputComponents = recipe->OutputComponents();

            // check input
            bool hasNecessaryResources = true;
            foreachitemconst(inputComponent, inputComponents)
            {
                if (producerStorage->GetResourceQuantity(inputComponent.first) < inputComponent.first)
                {
                    hasNecessaryResources = false;
                    break;
                }
            }

            if (!hasNecessaryResources)
                continue;

            // check output space
            const u32 availableSpace = producerStorage->GetRemainingStorageSpace();
            u32 wantedSpace = 0;
            foreachitemconst(outputComponent, outputComponents) { wantedSpace += outputComponent.second; }
            if (wantedSpace > availableSpace)
                continue;

            // on est ici, on est bon. On lance la production: On consomme les input et on set le temps de prod
            foreachitemconst(inputComponent, inputComponents)
            {
                const u32 removedResource = producerStorage->RemoveResource(inputComponent.first, inputComponent.second);
                AlwaysCheckedAssert(removedResource == inputComponent.second);
            }

            producer.SetProductionTimeRemaining(recipe->CraftDuration());
            producer.SetState(RecipeProductionState::PRODUCING);

            break;
        }
        case RecipeProductionState::PRODUCING:
        {
            const float remainingTime = producer.ProductionTimeRemaining();
            if (remainingTime <= 0.f)
            {
                // produce
                MemoryView<const RecipeComponent> outputComponents = recipe->OutputComponents();
                foreachitemconst(recipeComponent, outputComponents)
                {
                    const u32 addedResource = producerStorage->AddResource(recipeComponent.first, recipeComponent.second);
                    AlwaysCheckedAssert(addedResource == recipeComponent.second);
                }
                producer.SetState(RecipeProductionState::IDLE);
            }
            else
            {
                producer.SetProductionTimeRemaining(remainingTime - TimeManager::GameplayDeltaTime());
            }
            break;
        }
        }
    }
}

} // namespace ECSEngine
