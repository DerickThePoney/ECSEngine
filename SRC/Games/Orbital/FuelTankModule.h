
#pragma once
#include "ECSCore/Module.h"
#include "ECSCore/ModuleTemplate.h"

namespace ECSEngine
{
class FuelTankModuleTemplate : public ModuleTemplate
{
    DECLARE_MODULE_TEMPLATE(FuelTankModule, FuelTankModuleTemplate);

public:
    FuelTankModuleTemplate()
        : ModuleTemplate()
    {
    }
    ~FuelTankModuleTemplate() { }

    Module* CreateInstance(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) const override;

    SERIALIZE() { PROPERTYFIELD(MaxFuelMass, 0.f); }

    float MaxFuelMass() const { return FMaxFuelMass; }

protected:
    void VirtualDrawEditor() override;

private:
    float FMaxFuelMass = 0.f;
};

class FuelTankModule : public Module
{
    DECLARE_MODULE(FuelTankModule);

    DECLARE_SAVELOAD_ABILITIES();

public:
    FuelTankModule();
    ~FuelTankModule();

    float CurrentRemainingFuelMass() const { return FCurrentRemainingFuelMass; }
    void SetCurrentRemainingFuelMass(const float Value) { FCurrentRemainingFuelMass = Value; }

    float RemainingFuelMassRatio() const;

    float GetMaxFuelMass() const;

    float DryMass() const { return FDryMass; }
    void SetDrayMass(const float Value);
    bool DryMassSet() const { return FDryMassSet; }

    bool IsTankEmpty() const;

protected:
    void VirtualInit(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) override;

private:
    float FCurrentRemainingFuelMass = 0.f;
    float FDryMass = 0.f;
    bool FDryMassSet = false;
};

} // namespace ECSEngine
