
#include "stdafx.h"

#include "KeplerWarpModule.h"

#include "Common/SavingSystemImplementation.h"
#include "ECSCore/EntityTemplateManagerMethods.h"
#include "ECSCore/ModuleUtils.h"

CEREAL_REGISTER_TYPE(ECSEngine::KeplerWarpModuleTemplate);
CEREAL_REGISTER_POLYMORPHIC_RELATION(ECSEngine::ModuleTemplate, ECSEngine::KeplerWarpModuleTemplate);

namespace ECSEngine
{
IMPLEMENT_MODULE_TEMPLATE(KeplerWarpModule, KeplerWarpModuleTemplate);

Module* KeplerWarpModuleTemplate::CreateInstance(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) const
{
    return NewModule<KeplerWarpModule>(this, parUnitId, parParameters);
}

void KeplerWarpModuleTemplate::VirtualDrawEditor()
{
}

IMPLEMENT_SAVELOAD_ABILITIES(KeplerWarpModule);
template<typename Chunk, bool isWriting>
void KeplerWarpModule::SaveLoad(Chunk& parChunk)
{
    parent_type::SaveLoad(parChunk);
}

KeplerWarpModule::KeplerWarpModule()
    : Module()
{
}

KeplerWarpModule::~KeplerWarpModule()
{
}

} // namespace ECSEngine
