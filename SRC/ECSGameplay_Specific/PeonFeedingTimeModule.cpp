#include "stdafx.h"

#include "PeonFeedingTimeModule.h"

#include "Application/PropertyDrawer.h"
#include "ECSCore/EntityTemplateManagerMethods.h"
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
    EDITOR_PROPERTY_WITH_LIMITS("Time between two feed times", FTimeBetweenTwoFeedTime, 0.f, 120.f); // A push dans un domaine PLAYER DATA ou un machin du genre
}

PeonFeedingTimeModule::PeonFeedingTimeModule()
{
}

void PeonFeedingTimeModule::VirtualInit(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters)
{
    parent_type::VirtualInit(parUnitId, parParameters);
    FTimeBetweenFeedingTimes.SetInitialValue(Template<PeonFeedingTimeModuleTemplate>()->TimeBetweenTwoFeedTime());
    FRemainingTimeBetweenTwoFeedTimes = FTimeBetweenFeedingTimes.ComputedValue();
}

} // namespace ECSEngine
