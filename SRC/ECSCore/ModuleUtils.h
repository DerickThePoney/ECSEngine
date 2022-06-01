#pragma once

namespace ECSEngine
{
class Module;
class ModuleTemplate;
class EntityId;

namespace ModuleParameters
{
class ParameterContainer;
}
template<typename T>
Module* NewModule(const ModuleTemplate* parTemplate, const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters);
} // namespace ECSEngine
