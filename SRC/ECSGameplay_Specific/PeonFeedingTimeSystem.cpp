#include "stdafx.h"

#include "PeonFeedingTimeSystem.h"

#include "ColonyPeonsManagementModule.h"
#include "Common/TimeManager.h"
#include "ECSCore/EntityFactory.h"
#include "ECSCore/ModuleAccessor.h"
#include "ECSGameplay_Common/EntityLinksModules.h"
#include "ECSGameplay_Common/PositionModule.h"
#include "GameResources.h"
#include "GameplayConstants.h"
#include "PeonFeedingTimeModule.h"
#include "ResourceStorageModule.h"

namespace ECSEngine
{

PeonFeedingTimeSystem::PeonFeedingTimeSystem()
    : ModuleSystem()
{
    RegisterDepency<ColonyPeonsManagementModule>(Worlds::COLONY);
    RegisterDepency<PeonFeedingTimeModule>(Worlds::COLONY);
    RegisterDepency<ResourceStorageModule>(Worlds::COLONY);
    RegisterDepency<PositionModule>(Worlds::PEONS);
    RegisterDepency<PositionModule>(Worlds::COLONY);
}

PeonFeedingTimeSystem::~PeonFeedingTimeSystem()
{
}

void PeonFeedingTimeSystem::VirtualUpdate()
{
    ModuleSystem::VirtualUpdate();

    ModuleAccessor<PeonFeedingTimeModule> lifeSpanAccessor(Worlds::COLONY);
    ModuleAccessor<ColonyPeonsManagementModule> colonyManagerAccessor(Worlds::COLONY);
    ModuleAccessor<ResourceStorageModule> colonyStorageAccessor(Worlds::COLONY);
    ModuleAccessor<PositionModule> colonyPositionAccessor(Worlds::COLONY);
    ModuleAccessor<PositionModule> peonPositionAccessor(Worlds::PEONS);

    // Idées:
    //    - Chaque span, on nourrit les peons. X points de nourriture par peons.
    //    - Si on peut pas consummer nbPeons x X points de nourriture, on compte le max de peons qu'on peut nourrir. Ceux qui sont le plus de loin de la colony meurent.
    std::vector<EntityId> deadIds;
    foreachitem(lifeSpan, lifeSpanAccessor)
    {
        const float newLifeSpan = lifeSpan.RemainingTimeBeforeNextFeed() - TimeManager::FrameDeltaTime();
        lifeSpan.SetRemainingTimeBeforeNextFeedingTime(newLifeSpan);

        if (newLifeSpan <= 0.f)
        {
            ColonyPeonsManagementModule* peonManager = colonyManagerAccessor[lifeSpan.UnitId()];
            AssertRelease(peonManager != nullptr);

            const u32 peonsInColony = peonManager->PeonsInColony();
            const u32 foodNecessary = peonsInColony * GameplayConstants::PeonFeeding::PeonEatQuantity;

            ResourceStorageModule* colonyStorage = colonyStorageAccessor[lifeSpan.UnitId()];
            AssertRelease(colonyStorage != nullptr);
            const u32 resourceInStorage = colonyStorage->GetResourceQuantity(GameResource::FOOD);

            u32 foodToEat = foodNecessary;
            u32 peonsToKill = 0;
            if (resourceInStorage < foodToEat)
            {
                foodToEat = resourceInStorage - resourceInStorage % GameplayConstants::PeonFeeding::PeonEatQuantity;
                peonsToKill = (foodNecessary - foodToEat) / GameplayConstants::PeonFeeding::PeonEatQuantity;
            }
            AlwaysCheckedAssert(peonsToKill <= peonsInColony);
            const u32 eaten = colonyStorage->RemoveResource(GameResource::FOOD, foodToEat);
            AlwaysCheckedAssert(eaten == foodToEat);

            lifeSpan.ResetFeedingTime();

            if (peonsToKill == 0)
                continue;

            const PositionModule* colonyPositionModule = colonyPositionAccessor[lifeSpan.UnitId()];
            AssertRelease(colonyPositionModule != nullptr);
            const glm::vec2 colonyPosition = glm::xz(colonyPositionModule->GetPosition3D());

            std::vector<std::pair<float, EntityId>> peonsPositions;
            peonsPositions.reserve(peonsInColony);
            foreachitemconst(peon, peonManager->IdlePeons())
            {
                const PositionModule* posMod = peonPositionAccessor[peon];
                AssertRelease(posMod != nullptr);

                peonsPositions.push_back({ glm::length2(glm::xz(posMod->GetPosition3D()) - colonyPosition), peon });
            }

            foreachitemconst(peon, peonManager->OccupiedPeons())
            {
                const PositionModule* posMod = peonPositionAccessor[peon];
                AssertRelease(posMod != nullptr);

                peonsPositions.push_back({ glm::length2(glm::xz(posMod->GetPosition3D()) - colonyPosition), peon });
            }

            std::sort(peonsPositions.begin(), peonsPositions.end(), [](const auto& a, const auto& b) { return a.first > b.first; });

            // TODO KILL PEONS
            forrange(i, 0, peonsToKill)
            {
                peonManager->RemovePeon(peonsPositions[i].second);
                EntityFactory::MarkEntityAsDead(peonsPositions[i].second);
            }
        }
    }
}

} // namespace ECSEngine
