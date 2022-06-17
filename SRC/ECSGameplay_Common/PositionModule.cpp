#include "stdafx.h"

#include "PositionModule.h"

#include "Common/SavingSystemImplementation.h"
#include "ECSCore/EntityTemplateManagerMethods.h"
#include "ECSCore/ModuleParameters.h"
#include "ECSCore/ModuleUtils.h"

CEREAL_REGISTER_TYPE(ECSEngine::PositionModuleTemplate);
CEREAL_REGISTER_POLYMORPHIC_RELATION(ECSEngine::ModuleTemplate, ECSEngine::PositionModuleTemplate)

namespace ECSEngine
{

IMPLEMENT_MODULE_TEMPLATE(PositionModule, PositionModuleTemplate);

Module* PositionModuleTemplate::CreateInstance(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) const
{
    return NewModule<PositionModule>(this, parUnitId, parParameters);
}

void PositionModuleTemplate::VirtualDrawEditor()
{
}

void PositionModule::VirtualInit(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters)
{
    parent_type::VirtualInit(parUnitId, parParameters);

    FPosition = parParameters.Get_IFP<ModuleParameters::Position>(glm::vec3(0.0f));
}

IMPLEMENT_SAVELOAD_ABILITIES(PositionModule);
template<typename Chunk, bool isWriting>
void PositionModule::SaveLoad(Chunk& parChunk)
{
    parChunk& FPosition;
}

} // namespace ECSEngine
