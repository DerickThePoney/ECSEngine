#include "stdafx.h"

#include "PositionModule.h"

#include "Common/SaveLoadData.h"
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

template<>
void SaveLoad<SavingSystem::SaveChunk, PositionModule, true>(SavingSystem::SaveChunk& parChunk, PositionModule& parValue)
{
    parChunk.GetBuffer().WriteGuards(typeid(PositionModule).hash_code());
    parValue.SaveLoad<SavingSystem::SaveChunk, true>(parChunk);
}

template<>
void SaveLoad<SavingSystem::ReadChunk, PositionModule, false>(SavingSystem::ReadChunk& parChunk, PositionModule& parValue)
{
    u32 id = parChunk.GetBuffer().ReadId();
    u32 size = parChunk.GetBuffer().ReadSize();

    u32 expectedId = typeid(PositionModule).hash_code();
    AlwaysCheckedAssertMsg(id == expectedId, "SaveFile is probably corrupted as the id for PositionModule does not match the id we got !");
    AlwaysCheckedAssert(size == 0, "Got non zero size while reading guards...");

    parValue.SaveLoad<SavingSystem::ReadChunk, false>(parChunk);
}

template<typename Chunk, bool isWriting>
void PositionModule::SaveLoad(Chunk& parChunk)
{
    parChunk& FPosition;
}

template void PositionModule::SaveLoad<SavingSystem::SaveChunk, true>(SavingSystem::SaveChunk& parChunk);
template void PositionModule::SaveLoad<SavingSystem::ReadChunk, false>(SavingSystem::ReadChunk& parChunk);

} // namespace ECSEngine
