#pragma once
#include "EntityId.h"
#include "EntityWorld.h"
#include "ModuleController.h"
#include "ModuleParameters.h"
#include "ModuleTemplate.h"
#include "WorldManager.h"

namespace ECSEngine
{
template<typename T>
Module* NewModule(const ModuleTemplate* parTemplate, const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters)
{
    EntityWorld& world = WorldManager::Instance().GetWorld(parUnitId.GetWorld());
    IModuleController* controller = world.GetControllerIFP<T>();
    AssertRelease(controller != nullptr);
    controller->AllocateForEntity(parUnitId);
    Module* mod = controller->GetModulePtrForEntity(parUnitId);
    mod->Init(parTemplate, parUnitId, parParameters);
    return mod;
}
} // namespace ECSEngine
