#include "stdafx.h"

#include "PeonLifeSpanSystem.h"

#include "ColonyPeonsManagementModule.h"
#include "Common/TimeManager.h"
#include "ECSCore/EntityFactory.h"
#include "ECSCore/ModuleAccessor.h"
#include "ECSGameplay_Common/EntityLinksModules.h"
#include "PeonLifeSpanModule.h"

namespace ECSEngine
{

PeonLifeSpanSystem::PeonLifeSpanSystem()
    : ModuleSystem()
{
    RegisterDepency<ColonyPeonsManagementModule>(Worlds::COLONY);

    RegisterDepency<PeonLifeSpanModule>(Worlds::PEONS);
    RegisterDepency<LinkToOwnerModule>(Worlds::PEONS);
}

PeonLifeSpanSystem::~PeonLifeSpanSystem()
{
}

void PeonLifeSpanSystem::VirtualUpdate()
{
    ModuleSystem::VirtualUpdate();

    ModuleAccessor<PeonLifeSpanModule> lifeSpanAccessor(Worlds::PEONS);
    ModuleAccessor<LinkToOwnerModule> ownerAccessor(Worlds::PEONS);
    ModuleAccessor<ColonyPeonsManagementModule> colonyManagerAccessor(Worlds::COLONY);

    std::vector<EntityId> deadIds;
    foreachitem(lifeSpan, lifeSpanAccessor)
    {
        const float newLifeSpan = lifeSpan.RemainingLifeSpan() - TimeManager::FrameDeltaTime();
        lifeSpan.SetRemainingLifeSpan(newLifeSpan);

        if (newLifeSpan <= 0.f)
            deadIds.push_back(lifeSpan.UnitId());
    }

    foreachitemconst(dead, deadIds)
    {
        const LinkToOwnerModule* linkToOwner = ownerAccessor[dead];
        AssertRelease(linkToOwner != nullptr);
        AssertRelease(linkToOwner->OwnerId().Valid());
        ColonyPeonsManagementModule* peonManager = colonyManagerAccessor[linkToOwner->OwnerId()];
        AssertRelease(peonManager != nullptr);

        peonManager->RemovePeon(dead);

        EntityFactory::MarkEntityAsDead(dead);
    }
}

} // namespace ECSEngine