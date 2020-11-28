#include "stdafx.h"

#include "ColonyPeonsTaskAsignmentSystem.h"

#include "ColonyPeonsManagementModule.h"
#include "Common/RandomGenerator.h"
#include "ECSCore/EntityId.h"
#include "ECSCore/ModuleAccessor.h"
#include "ECSGameplay_Common/PositionModule.h"
#include "GameResources.h"
#include "ResourceHarvesterModule.h"
#include "ResourceStorageModule.h"

#include <random>

namespace ECSEngine
{

namespace
{
EntityId GetTargetForPeon(const PositionModule& parPeonPositionModule,
      ModuleAccessor<PositionModule>& parResourcesPositionAccessor,
      ModuleAccessor<ResourceStorageModule>& parProducerResourceStorageAccessor,
      const std::set<EntityId>& parAssignedTargets)
{
    EntityId res;
    float closestDistance = std::numeric_limits<float>::max();
    foreachitemconst(position, parResourcesPositionAccessor)
    {
        if (parAssignedTargets.find(position.UnitId()) != parAssignedTargets.end())
            continue;

        const float sqDist = glm::length2(glm::xz(parPeonPositionModule.GetPosition3D() - position.GetPosition3D()));
        if (sqDist < closestDistance)
        {
            const ResourceStorageModule* storage = parProducerResourceStorageAccessor[position.UnitId()];
            if (storage->GetResourceQuantity(GameResource::LENGTH) > 0)
            {
                closestDistance = sqDist;
                res = position.UnitId();
            }
        }
    }

    return res;
}
} // namespace

ColonyPeonsTaskAssignmentSystem::ColonyPeonsTaskAssignmentSystem()
{
    // Colony
    RegisterDepency<ColonyPeonsManagementModule>(Worlds::COLONY);

    // resource producers
    RegisterDepency<PositionModule>(Worlds::RESOURCE_PROD);
    RegisterDepency<ResourceStorageModule>(Worlds::RESOURCE_PROD);

    // peons
    RegisterDepency<PositionModule>(Worlds::PEONS);
    RegisterDepency<ResourceHarvesterModule>(Worlds::PEONS);
}

ColonyPeonsTaskAssignmentSystem::~ColonyPeonsTaskAssignmentSystem()
{
}

void ColonyPeonsTaskAssignmentSystem::VirtualUpdate()
{
    ModuleSystem::VirtualUpdate();

    ModuleAccessor<ColonyPeonsManagementModule> colonyPeonsModuleAccessor(Worlds::COLONY);

    ModuleAccessor<PositionModule> resourcesPositionModuleAccessor(Worlds::RESOURCE_PROD);
    ModuleAccessor<PositionModule> peonsPositionModuleAccessor(Worlds::PEONS);

    ModuleAccessor<ResourceStorageModule> producerResourceStorageAccessor(Worlds::RESOURCE_PROD);
    ModuleAccessor<ResourceHarvesterModule> peonHarvesterAccessor(Worlds::PEONS);

    foreachitem(colonyPeonsModule, colonyPeonsModuleAccessor)
    {
        std::vector<EntityId>& idlePeons = colonyPeonsModule.IdlePeons();
        std::vector<EntityId> unIdlePeons;
        unIdlePeons.reserve(idlePeons.size());
        std::set<EntityId> assignedTargets;

        foreachitemconst(occupiedPeon, colonyPeonsModule.OccupiedPeons())
        {
            const ResourceHarvesterModule* harvesterModule = peonHarvesterAccessor[occupiedPeon];
            AssertRelease(harvesterModule != nullptr);
            assignedTargets.insert(harvesterModule->Target());
        }

        foreachitemconst(peon, idlePeons)
        {
            const PositionModule* peonPositionModule = peonsPositionModuleAccessor[peon];
            AssertRelease(peonPositionModule != nullptr);
            EntityId newTarget = GetTargetForPeon(*peonPositionModule, resourcesPositionModuleAccessor, producerResourceStorageAccessor, assignedTargets);
            if (!newTarget.Valid())
                continue;

            assignedTargets.insert(newTarget);

            ResourceHarvesterModule* peonHarvesterModule = peonHarvesterAccessor[peon];
            AssertRelease(peonHarvesterModule != nullptr);
            AlwaysCheckedAssert(peonHarvesterModule->Target() == EntityId());
            peonHarvesterModule->SetTarget(newTarget);

            unIdlePeons.push_back(peon);
        }

        foreachitemconst(peon, unIdlePeons) { colonyPeonsModule.SetPeonOccupied(peon); }
    }
}

} // namespace ECSEngine