
#include "stdafx.h"

#include "BoxColliderModule.h"

#include "Common/SavingSystemImplementation.h"
#include "ECSCore/EntityTemplateManagerMethods.h"
#include "ECSCore/ModuleUtils.h"

CEREAL_REGISTER_TYPE(ECSEngine::BoxColliderModuleTemplate);
CEREAL_REGISTER_POLYMORPHIC_RELATION(ECSEngine::ModuleTemplate, ECSEngine::BoxColliderModuleTemplate);

namespace ECSEngine
{
IMPLEMENT_MODULE_TEMPLATE(BoxColliderModule, BoxColliderModuleTemplate);

Module* BoxColliderModuleTemplate::CreateInstance(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) const
{
    return NewModule<BoxColliderModule>(this, parUnitId, parParameters);
}

void BoxColliderModuleTemplate::VirtualDrawEditor()
{
}

IMPLEMENT_SAVELOAD_ABILITIES(BoxColliderModule);
template<typename Chunk, bool isWriting>
void BoxColliderModule::SaveLoad(Chunk& parChunk)
{
    parent_type::SaveLoad(parChunk);
}

BoxColliderModule::BoxColliderModule()
    : Module()
{
}

BoxColliderModule::~BoxColliderModule()
{
}

void BoxColliderModule::VirtualInit(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters)
{
    parent_type::VirtualInit(parUnitId, parParameters);
}

void BoxColliderModule::VirtualOnLoaded()
{
    parent_type::VirtualOnLoaded();
}

void BoxColliderModule::VirtualPostInit()
{
    parent_type::VirtualPostInit();

    // ADD SHAPE TO THE PHYSICS RIGIDBODY
}

} // namespace ECSEngine
