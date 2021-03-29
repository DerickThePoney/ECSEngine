#include "stdafx.h"

#include "ColonyTraitsModule.h"

#include "ECSCore/EntityTemplateManager.h"
#include "ECSCore/ModuleUtils.h"
#include "GameplayConstants.h"

CEREAL_REGISTER_TYPE(ECSEngine::ColonyTraitsModuleTemplate);
CEREAL_REGISTER_POLYMORPHIC_RELATION(ECSEngine::ModuleTemplate, ECSEngine::ColonyTraitsModuleTemplate)

namespace ECSEngine
{
IMPLEMENT_MODULE_TEMPLATE(ColonyTraitsModule, ColonyTraitsModuleTemplate);

Module* ColonyTraitsModuleTemplate::CreateInstance(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) const
{
    return NewModule<ColonyTraitsModule>(this, parUnitId, parParameters);
}

void ColonyTraitsModuleTemplate::VirtualDrawEditor()
{
}

void ColonyTraitsModule::VirtualInit(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parContainer)
{
    parent_type::VirtualInit(parUnitId, parContainer);

    FInfluenceRange.SetInitialValue(GameplayConstants::Colony::ColonyInitialRange);
}

void ColonyTraitsModule::AddInfluenceModifier(const ValueModifier<float>& parModifer)
{
    FInfluenceRange.AddModifier(parModifer);
}

void ColonyTraitsModule::RemoveInfluenceModifier(const ValueModifier<float>& parModifier)
{
    FInfluenceRange.RemoveModifer(parModifier);
}

} // namespace ECSEngine
