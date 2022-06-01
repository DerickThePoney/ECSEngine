#include "stdafx.h"

#include "GameplayActions.h"

#include "ColonyPeonsManagementModule.h"
#include "ECSCore/EntityFactory.h"
#include "ECSCore/EntityTemplateManager.h"
#include "ECSCore/ModuleAccessor.h"
#include "ECSCore/ModuleParameters.h"
#include "ECSGameplay_Common/PositionModule.h"
#include "ResourceStorageModule.h"
#include "ECSGameplay_Common/WorldIds.h"

namespace ECSEngine
{

void SpawnPeonOrder::Execute()
{
    ModuleAccessor<ColonyPeonsManagementModule> colonyPeonsAccessor(EEntityWorlds::COLONY);
    ModuleAccessor<ResourceStorageModule> colonyResourceStorageAccessor(EEntityWorlds::COLONY);
    ModuleAccessor<PositionModule> colonyPositionAccessor(EEntityWorlds::COLONY);

    ColonyPeonsManagementModule* colonyPeons = colonyPeonsAccessor[FColonyId];
    AssertRelease(colonyPeons != nullptr);
    ResourceStorageModule* colonyStorage = colonyResourceStorageAccessor[FColonyId];
    AssertRelease(colonyStorage != nullptr);

    const u32 currentPeons = colonyPeons->PeonsInColony();
    std::vector<std::pair<GameResource::Type, u32>> costs(std::move(FPeonCostRule.CostForNextSpawn(currentPeons)));
    bool canExecute = true;
    foreachitemconst(resQ, costs)
    {
        const u32 resQuantity = colonyStorage->GetResourceQuantity(resQ.first);
        canExecute = canExecute && resQ.second <= resQuantity;
    }

    if (!canExecute)
        return;

    const PositionModule* colonyPos = colonyPositionAccessor[FColonyId];
    AssertRelease(colonyPos != nullptr);

    ModuleParameters::ParameterContainer container;
    container.Set<ModuleParameters::Position>(colonyPos->GetPosition3D());
    container.Set<ModuleParameters::OwnerId>(FColonyId);

    const EntityTemplate* et = FPeonCostRule.PeonTemplate();
    AssertRelease(et != nullptr);
    EntityId newPeon = EntityFactory::CreateEntity(et, container);
    colonyPeons->AddNewPeon(newPeon);

    foreachitemconst(resQ, costs)
    {
        const u32 payed = colonyStorage->RemoveResource(resQ.first, resQ.second);
        AlwaysCheckedAssert(payed == resQ.second);
    }
}

} // namespace ECSEngine
