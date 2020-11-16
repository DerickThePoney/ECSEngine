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

u32 PeonSpawnModuleTemplate::CostForNextSpawn(const u32 parCurrentPeonsQuantity) const
{
    return (u32)floor((float)FBaseCost * pow(FMultiplier, (float)parCurrentPeonsQuantity));
}

void PeonSpawnModuleTemplate::VirtualDrawEditor()
{
    EDITOR_PROPERTY_ENTITY_TEMPLATE("Peon template", FPeonTemplate, FPeonTemplateName);
    EDITOR_PROPERTY_WITH_LIMITS("Base peon cost", FBaseCost, 1u, 1000u);
    EDITOR_PROPERTY_WITH_LIMITS("Cost multiplier", FMultiplier, 1.f, 2.f);
}

void PeonSpawnModuleTemplate::VirtualPostLoad()
{
    SetUpTemplate();
}

void PeonSpawnModuleTemplate::SetUpTemplate()
{
    FPeonTemplate = EntityTemplateManager::Instance().GetEntityTemplate(FPeonTemplateName);
}

PeonSpawnModule::PeonSpawnModule()
    : Module()
{
}

PeonSpawnModule::~PeonSpawnModule()
{
}

u32 PeonSpawnModule::CostForNextSpawn(const u32 parCurrentPeonsQuantity) const
{
    return Template<PeonSpawnModuleTemplate>()->CostForNextSpawn(parCurrentPeonsQuantity);
}

GameResource::Type PeonSpawnModule::ResourceToPay() const
{
    return Template<PeonSpawnModuleTemplate>()->ResourceToPay();
}

void PeonSpawnModule::VirtualInit(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parContainer)
{
    parent_type::VirtualInit(parUnitId, parContainer);
}

} // namespace ECSEngine