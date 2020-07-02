#include "stdafx.h"

#include "EntityFactory.h"

#include "EntityTemplate.h"
#include "WorldManager.h"

namespace ECSEngine
{
namespace EntityFactory
{

const EntityId CreateEntity(const EntityTemplate* parTemplate, const ModuleParameters::ParameterContainer& parParameters)
{
    EntityWorld* world = WorldManager::Instance().GetWorldIFP(parTemplate->GetWorldId());
    AssertRelease(world != nullptr);

    EntityId unitId = world->CreateEntityFromTemplateReturnEntityId(parTemplate, parParameters);
    AssertRelease(unitId.Valid());
    return unitId;
}

} // namespace EntityFactory
} // namespace ECSEngine