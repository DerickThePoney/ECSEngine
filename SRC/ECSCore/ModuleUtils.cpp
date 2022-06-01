#include "stdafx.h"

#include "ModuleUtils.h"

#include "EntityId.h"
#include "EntityWorld.h"
#include "ModuleController.h"
#include "ModuleParameters.h"
#include "ModuleTemplate.h"
#include "WorldManager.h"
#include "Module.h"

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

#define DECLARE_MODULE_AND_TEMPLATE(NAME, TEMPLATE)                                                                                                                                \
    template Module* NewModule<NAME>(const ModuleTemplate* parTemplate, const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters);
#include "ModuleList.inl"
#undef DECLARE_MODULE_AND_TEMPLATE

}