#include "stdafx.h"

#include "ColonyPeonsTaskAsignmentSystem.h"

#include "ColonyPeonsManagementModule.h"
#include "ColonyTraitsModule.h"
#include "Common/RandomGenerator.h"
#include "ECSCore/EntityId.h"
#include "ECSCore/ModuleAccessor.h"
#include "ECSGameplay_Common/PositionModule.h"
#include "GameResources.h"
#include "ResourceHarvesterModule.h"
#include "ResourceStorageModule.h"

#include <random>
#include "ECSGameplay_Common/WorldIds.h"

namespace ECSEngine
{

namespace
{
EntityId GetTargetForPeon(const PositionModule& parPeonPositionModule,
      ModuleAccessor<PositionModule>& parResourcesPositionAccessor,
      ModuleAccessor<ResourceStorageModule>& parProducerResourceStorageAccessor,
      const std::set<EntityId>& parAssignedTargets,
      const float parMaxInfluence)
{
    EntityId res;
    float closestDistance = std::numeric_limits<float>::max();
    const float maxInfSq = parMaxInfluence * parMaxInfluence;
    foreachitemconst(position, parResourcesPositionAccessor)
    {
        if (parAssignedTargets.find(position.UnitId()) != parAssignedTargets.end())
            continue;

        const float sqDist = glm::length2(glm::xz(parPeonPositionModule.GetPosition3D() - position.GetPosition3D()));
        if (sqDist <= (maxInfSq) && sqDist < closestDistance)
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
    RegisterDepency<ColonyPeonsManagementModule>(EEntityWorlds::COLONY);
    RegisterDepency<ColonyTraitsModule>(EEntityWorlds::COLONY);

    // resource producers
    RegisterDepency<PositionModule>(EEntityWorlds::RESOURCE_PROD);
    RegisterDepency<ResourceStorageModule>(EEntityWorlds::RESOURCE_PROD);

    // peons
    RegisterDepency<PositionModule>(EEntityWorlds::PEONS);
    RegisterDepency<ResourceHarvesterModule>(EEntityWorlds::PEONS);
}

ColonyPeonsTaskAssignmentSystem::~ColonyPeonsTaskAssignmentSystem()
{
}

void ColonyPeonsTaskAssignmentSystem::VirtualUpdate()
{
    ModuleSystem::VirtualUpdate();

    ModuleAccessor<ColonyPeonsManagementModule> colonyPeonsModuleAccessor(EEntityWorlds::COLONY);
    ModuleAccessor<ColonyTraitsModule> colonyTraitsAccessor(EEntityWorlds::COLONY);

    ModuleAccessor<PositionModule> resourcesPositionModuleAccessor(EEntityWorlds::RESOURCE_PROD);
    ModuleAccessor<PositionModule> peonsPositionModuleAccessor(EEntityWorlds::PEONS);

    ModuleAccessor<ResourceStorageModule> producerResourceStorageAccessor(EEntityWorlds::RESOURCE_PROD);
    ModuleAccessor<ResourceHarvesterModule> peonHarvesterAccessor(EEntityWorlds::PEONS);

    foreachitem(colonyPeonsModule, colonyPeonsModuleAccessor)
    {
        std::vector<EntityId>& idlePeons = colonyPeonsModule.IdlePeons();
        std::vector<EntityId> unIdlePeons;
        unIdlePeons.reserve(idlePeons.size());
        std::set<EntityId> assignedTargets;

        const ColonyTraitsModule* colonyTraitsModule = colonyTraitsAccessor[colonyPeonsModule.UnitId()];
        AssertRelease(colonyTraitsModule != nullptr);
        const float maxInfluence = colonyTraitsModule->InfluenceRange();

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
            EntityId newTarget = GetTargetForPeon(*peonPositionModule, resourcesPositionModuleAccessor, producerResourceStorageAccessor, assignedTargets, maxInfluence);
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
