
#include "stdafx.h"

#include "PhysicsFloatingOriginModule.h"

#include "Common/SavingSystemImplementation.h"
#include "ECSCore/EntityTemplateManagerMethods.h"
#include "ECSCore/ModuleUtils.h"

CEREAL_REGISTER_TYPE(ECSEngine::PhysicsFloatingOriginModuleTemplate);
CEREAL_REGISTER_POLYMORPHIC_RELATION(ECSEngine::ModuleTemplate, ECSEngine::PhysicsFloatingOriginModuleTemplate);

namespace ECSEngine
{
IMPLEMENT_MODULE_TEMPLATE(PhysicsFloatingOriginModule, PhysicsFloatingOriginModuleTemplate);

Module* PhysicsFloatingOriginModuleTemplate::CreateInstance(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) const
{
    return NewModule<PhysicsFloatingOriginModule>(this, parUnitId, parParameters);
}

void PhysicsFloatingOriginModuleTemplate::VirtualDrawEditor()
{
    EDITOR_PROPERTY_SIMPLE("Floating origin bubble radius", FBubbleRadius);

    if (FBubbleRadius < 100.0)
    {
        FBubbleRadius = 100.0;
    }
}

IMPLEMENT_SAVELOAD_ABILITIES(PhysicsFloatingOriginModule);
template<typename Chunk, bool isWriting>
void PhysicsFloatingOriginModule::SaveLoad(Chunk& parChunk)
{
    parent_type::SaveLoad(parChunk);

    parChunk & FOrigin;
}

PhysicsFloatingOriginModule::PhysicsFloatingOriginModule()
    : Module()
{
}

PhysicsFloatingOriginModule::~PhysicsFloatingOriginModule()
{
}

const double PhysicsFloatingOriginModule::BubbleRadius() const
{
    const PhysicsFloatingOriginModuleTemplate* const Temp = Template<PhysicsFloatingOriginModuleTemplate>();
    return Temp->BubbleRadius();
}

} // namespace ECSEngine
