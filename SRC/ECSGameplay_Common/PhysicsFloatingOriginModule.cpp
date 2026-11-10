
#include "stdafx.h"

#include "PhysicsFloatingOriginModule.h"

#include "Common/SavingSystemImplementation.h"
#include "ECSCore/EntityTemplateManagerMethods.h"
#include "ECSCore/ModuleParameters.h"
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

    if (!isWriting)
        SetBubbleRadius(Template<PhysicsFloatingOriginModuleTemplate>()->BubbleRadius());
}

PhysicsFloatingOriginModule::PhysicsFloatingOriginModule()
    : Module()
    , FOrigin(0.0)
{
}

PhysicsFloatingOriginModule::~PhysicsFloatingOriginModule()
{
}

void PhysicsFloatingOriginModule::VirtualInit(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters)
{
    parent_type::VirtualInit(parUnitId, parParameters);

    const PhysicsFloatingOriginModuleTemplate* const Temp = Template<PhysicsFloatingOriginModuleTemplate>();
    SetBubbleRadius(Temp->BubbleRadius());
}

void PhysicsFloatingOriginModule::SetBubbleRadius(double Radius)
{
    if (Radius < 50.0)
        Radius = 50.0;
    if (Radius > 20000.0)
        Radius = 20000.0;
    FBubbleRadius = Radius;
}

} // namespace ECSEngine
