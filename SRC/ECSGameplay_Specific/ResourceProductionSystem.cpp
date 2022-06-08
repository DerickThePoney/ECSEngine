#include "stdafx.h"

#include "ResourceProductionSystem.h"

#include "Common/TimeManager.h"
#include "ECSCore/ModuleAccessor.h"
#include "ECSCore/WorldIds.h"
#include "RecipeProductionModule.h"
#include "ResourceStorageModule.h"

namespace ECSEngine
{

ResourceProductionSystem::ResourceProductionSystem()
    : ModuleSystem()
{
    RegisterDepency<RecipeProductionModule>(EEntityWorlds::BUILDINGS);
    RegisterDepency<ResourceStorageModule>(EEntityWorlds::BUILDINGS);
}

ResourceProductionSystem::~ResourceProductionSystem()
{
}

void ResourceProductionSystem::VirtualUpdate()
{
    SCOPED_PROFILE_SIMPLE;
    ModuleSystem::VirtualUpdate();

    ModuleAccessor<ResourceStorageModule> resourceStorageAccesor(EEntityWorlds::BUILDINGS);
    ModuleAccessor<RecipeProductionModule> recipeProductionAccessor(EEntityWorlds::BUILDINGS);

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
                if (producerStorage->GetResourceQuantity(inputComponent.first) < inputComponent.second)
                {
                    hasNecessaryResources = false;
                    break;
                }
            }

            if (!hasNecessaryResources)
                continue;

            // check output space
            const u32 availableSpace = producerStorage->GetRemainingStorageSpace();
            const u32 wantedSpace = recipe->TotalQuantityOfOutputResourcesNecessary();
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
