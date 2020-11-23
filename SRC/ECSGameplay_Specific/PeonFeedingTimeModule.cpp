#include "stdafx.h"

#include "PeonFeedingTimeModule.h"

#include "Application/PropertyDrawer.h"
#include "ECSCore/EntityTemplateManager.h"
#include "ECSCore/ModuleUtils.h"

CEREAL_REGISTER_TYPE(ECSEngine::PeonFeedingTimeModuleTemplate);
CEREAL_REGISTER_POLYMORPHIC_RELATION(ECSEngine::ModuleTemplate, ECSEngine::PeonFeedingTimeModuleTemplate)

namespace ECSEngine
{
IMPLEMENT_MODULE_TEMPLATE(PeonFeedingTimeModule, PeonFeedingTimeModuleTemplate);

Module* PeonFeedingTimeModuleTemplate::CreateInstance(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) const
{
    return NewModule<PeonFeedingTimeModule>(this, parUnitId, parParameters);
}

void PeonFeedingTimeModuleTemplate::VirtualDrawEditor()
{
    EDITOR_PROPERTY_WITH_LIMITS("Initial Life Span", FInitialLifeSpan, 0.f, 120.f);
}

PeonFeedingTimeModule::PeonFeedingTimeModule()
{
}

void PeonFeedingTimeModule::VirtualInit(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters)
{
    parent_type::VirtualInit(parUnitId, parParameters);
    FRemainingLifeSpan = Template<PeonFeedingTimeModuleTemplate>()->InitialLifeSpan();
}

} // namespace ECSEngine
