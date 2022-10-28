#include "stdafx.h"

#include "EntityFactory.h"

#include "EntityTemplate.h"
#include "EntityWorld.h"
#include "WorldManager.h"

namespace ECSEngine
{
namespace EntityFactory
{

const EntityId RequestCreateEntity(const EntityTemplate* parTemplate, const ModuleParameters::ParameterContainer& parParameters)
{
    EntityWorld* world = WorldManager::Instance().GetWorldIFP(parTemplate->GetWorldId());
    AssertRelease(world != nullptr);

    EntityId unitId = world->CreateNewEntityId();
    AssertRelease(unitId.Valid());

    WorldManager::Instance().RequestCreateEntity(unitId, parTemplate, parParameters);
    return unitId;
}

void MarkEntityAsDead(const EntityId& parId)
{
    if (!parId.Valid())
        return;

    WorldManager::Instance().MarkAsDead(parId);
}

} // namespace EntityFactory
} // namespace ECSEngine
