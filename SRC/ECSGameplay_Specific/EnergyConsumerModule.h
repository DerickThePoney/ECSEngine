
#pragma once
#include "ECSCore/Module.h"
#include "ECSCore/ModuleTemplate.h"

namespace ECSEngine
{
class EnergyConsumerModuleTemplate : public ModuleTemplate
{
    DECLARE_MODULE_TEMPLATE(EnergyConsumerModule, EnergyConsumerModuleTemplate);

public:
    EnergyConsumerModuleTemplate()
        : ModuleTemplate()
    {
    }
    ~EnergyConsumerModuleTemplate() { }

    Module* CreateInstance(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) const override;

    u32 ConsumedEnergy() const { return FConsumedEnergy; }

    SERIALIZE() { PROPERTYFIELD(ConsumedEnergy, 0); }

protected:
    void VirtualDrawEditor() override;

private:
    u32 FConsumedEnergy;
};

class EnergyConsumerModule : public Module
{
    DECLARE_MODULE(EnergyConsumerModule);

public:
    EnergyConsumerModule();
    ~EnergyConsumerModule();

    u32 ConsumedEnergy() const;
};

} // namespace ECSEngine
