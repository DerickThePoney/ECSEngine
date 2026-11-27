
#include "stdafx.h"

#include "NozzleEngineModule.h"

#include "Common/SavingSystemImplementation.h"
#include "ECSCore/ECSCorePropertyDrawer.h"
#include "ECSCore/EntityTemplateManagerMethods.h"
#include "ECSCore/ModuleUtils.h"

CEREAL_REGISTER_TYPE(ECSEngine::NozzleEngineModuleTemplate);
CEREAL_REGISTER_POLYMORPHIC_RELATION(ECSEngine::ModuleTemplate, ECSEngine::NozzleEngineModuleTemplate);

namespace ECSEngine
{
IMPLEMENT_MODULE_TEMPLATE(NozzleEngineModule, NozzleEngineModuleTemplate);

Module* NozzleEngineModuleTemplate::CreateInstance(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) const
{
    return NewModule<NozzleEngineModule>(this, parUnitId, parParameters);
}

void NozzleEngineModuleTemplate::VirtualDrawEditor()
{
    EDITOR_PROPERTY_SIMPLE("Max thrust", FMaxThrust);
    if (FMaxThrust < 0.f)
        FMaxThrust = 0.f;

    EDITOR_PROPERTY_SIMPLE("Specific Impulse", FISP);
    if (FISP < 1.f)
        FISP = 1.f;

    EDITOR_PROPERTY_BOOL("Is a booster", FOffOnBoosters);
}

IMPLEMENT_SAVELOAD_ABILITIES(NozzleEngineModule);
template<typename Chunk, bool isWriting>
void NozzleEngineModule::SaveLoad(Chunk& parChunk)
{
    parent_type::SaveLoad(parChunk);

    parChunk & FThrottle;
}

NozzleEngineModule::NozzleEngineModule()
    : Module()
{
}

NozzleEngineModule::~NozzleEngineModule()
{
}

float NozzleEngineModule::MaxThrust() const
{
    const NozzleEngineModuleTemplate* const Temp = Template<NozzleEngineModuleTemplate>();
    AssertRelease(Temp != nullptr);
    return Temp->MaxThrust();
}

float NozzleEngineModule::ISP() const
{
    const NozzleEngineModuleTemplate* const Temp = Template<NozzleEngineModuleTemplate>();
    AssertRelease(Temp != nullptr);
    return Temp->ISP();
}

bool NozzleEngineModule::OffOnBoosters() const
{
    const NozzleEngineModuleTemplate* const Temp = Template<NozzleEngineModuleTemplate>();
    AssertRelease(Temp != nullptr);
    return Temp->OffOnBoosters();
}

float NozzleEngineModule::FuelFlow() const
{
    return FThrottle * MaxThrust() / (ISP() * 9.80665);
}

void NozzleEngineModule::SetThrottle(float Value)
{
    if (OffOnBoosters())
    {
        if (Value > 0.f)
        {
            FThrottle = 1.f;
        }
        return;
    }

    FThrottle = Clamp01(Value);
}

} // namespace ECSEngine
