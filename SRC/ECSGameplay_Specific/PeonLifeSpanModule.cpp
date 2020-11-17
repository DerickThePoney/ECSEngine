#include "stdafx.h"

#include "PeonLifeSpanModule.h"

#include "Application/PropertyDrawer.h"
#include "ECSCore/EntityTemplateManager.h"
#include "ECSCore/ModuleUtils.h"

CEREAL_REGISTER_TYPE(ECSEngine::PeonLifeSpanModuleTemplate);
CEREAL_REGISTER_POLYMORPHIC_RELATION(ECSEngine::ModuleTemplate, ECSEngine::PeonLifeSpanModuleTemplate)

namespace ECSEngine
{
IMPLEMENT_MODULE_TEMPLATE(PeonLifeSpanModule, PeonLifeSpanModuleTemplate);

Module* PeonLifeSpanModuleTemplate::CreateInstance(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) const
{
    return NewModule<PeonLifeSpanModule>(this, parUnitId, parParameters);
}

void PeonLifeSpanModuleTemplate::VirtualDrawEditor()
{
    EDITOR_PROPERTY_WITH_LIMITS("Initial Life Span", FInitialLifeSpan, 0.f, 120.f);
}

PeonLifeSpanModule::PeonLifeSpanModule()
{
}

void PeonLifeSpanModule::VirtualInit(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters)
{
    parent_type::VirtualInit(parUnitId, parParameters);
    FRemainingLifeSpan = Template<PeonLifeSpanModuleTemplate>()->InitialLifeSpan();
}

} // namespace ECSEngine
