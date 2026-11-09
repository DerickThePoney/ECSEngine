
#include "stdafx.h"

#include "FuelTankModule.h"

#include "Common/SavingSystemImplementation.h"
#include "ECSCore/EntityTemplateManagerMethods.h"
#include "ECSCore/ModuleUtils.h"

CEREAL_REGISTER_TYPE(ECSEngine::FuelTankModuleTemplate);
CEREAL_REGISTER_POLYMORPHIC_RELATION(ECSEngine::ModuleTemplate, ECSEngine::FuelTankModuleTemplate);

namespace ECSEngine
{
IMPLEMENT_MODULE_TEMPLATE(FuelTankModule, FuelTankModuleTemplate);

Module* FuelTankModuleTemplate::CreateInstance(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) const
{
    return NewModule<FuelTankModule>(this, parUnitId, parParameters);
}

void FuelTankModuleTemplate::VirtualDrawEditor()
{
    EDITOR_PROPERTY_SIMPLE("Max/initial fuel mass", FMaxFuelMass);
    if (FMaxFuelMass < 0.f)
    {
        FMaxFuelMass = 0.f;
    }
}

IMPLEMENT_SAVELOAD_ABILITIES(FuelTankModule);
template<typename Chunk, bool isWriting>
void FuelTankModule::SaveLoad(Chunk& parChunk)
{
    parent_type::SaveLoad(parChunk);

    parChunk & FCurrentRemainingFuelMass;
}

FuelTankModule::FuelTankModule()
    : Module()
{
}

FuelTankModule::~FuelTankModule()
{
}

float FuelTankModule::RemainingFuelMassRatio() const
{
    return Clamp01(FCurrentRemainingFuelMass / GetMaxFuelMass());
}

float FuelTankModule::GetMaxFuelMass() const
{
    const FuelTankModuleTemplate* Temp = Template<FuelTankModuleTemplate>();
    return Temp->MaxFuelMass();
}

void FuelTankModule::SetDrayMass(const float Value)
{
    if (!FDryMassSet)
    {
        FDryMass = Value;
        FDryMassSet = true;
    }
}

bool FuelTankModule::IsTankEmpty() const
{
    return FCurrentRemainingFuelMass < 1e-6f;
}

void FuelTankModule::VirtualInit(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters)
{
    parent_type::VirtualInit(parUnitId, parParameters);

    FCurrentRemainingFuelMass = GetMaxFuelMass();
}

} // namespace ECSEngine
