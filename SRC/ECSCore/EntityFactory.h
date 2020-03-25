#pragma once
#include "EntityId.h"

namespace ECSEngine
{
class EntityTemplate;
namespace ModuleParameters
{
class ParameterContainer;
}

namespace EntityFactory
{
const EntityId CreateEntity(const EntityTemplate* parTemplate, const ModuleParameters::ParameterContainer& parParameters);
}
} // namespace ECSEngine
