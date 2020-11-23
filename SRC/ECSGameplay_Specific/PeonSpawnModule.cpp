#include "stdafx.h"

#include "PeonSpawnModule.h"

#include "Application/PropertyDrawer.h"
#include "ECSCore/ECSCorePropertyDrawer.h"
#include "ECSCore/EntityTemplateManager.h"
#include "ECSCore/ModuleUtils.h"

CEREAL_REGISTER_TYPE(ECSEngine::PeonSpawnModuleTemplate);
CEREAL_REGISTER_POLYMORPHIC_RELATION(ECSEngine::ModuleTemplate, ECSEngine::PeonSpawnModuleTemplate)

namespace ECSEngine
{
IMPLEMENT_MODULE_TEMPLATE(PeonSpawnModule, PeonSpawnModuleTemplate);

Module* PeonSpawnModuleTemplate::CreateInstance(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) const
{
    return NewModule<PeonSpawnModule>(this, parUnitId, parParameters);
}

void PeonSpawnModuleTemplate::VirtualDrawEditor()
{
}

PeonSpawnModule::PeonSpawnModule()
    : Module()
{
}

PeonSpawnModule::~PeonSpawnModule()
{
}

void PeonSpawnModule::VirtualInit(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parContainer)
{
    parent_type::VirtualInit(parUnitId, parContainer);
}

} // namespace ECSEngine