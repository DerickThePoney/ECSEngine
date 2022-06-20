
#pragma once
#include "ECSCore/Module.h"
#include "ECSCore/ModuleTemplate.h"

namespace ECSEngine
{
class EnergyProducerModuleTemplate : public ModuleTemplate
{
    DECLARE_MODULE_TEMPLATE(EnergyProducerModule, EnergyProducerModuleTemplate);

public:
    EnergyProducerModuleTemplate()
        : ModuleTemplate()
    {
    }
    ~EnergyProducerModuleTemplate() { }

    Module* CreateInstance(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) const override;

    SERIALIZE() { PROPERTYFIELD(ProducedEnergy, 0); }

    u32 ProducedEnergy() const { return FProducedEnergy; }

protected:
    void VirtualDrawEditor() override;

private:
    u32 FProducedEnergy = 0;
};

class EnergyProducerModule : public Module
{
    DECLARE_MODULE(EnergyProducerModule);

    DECLARE_SAVELOAD_ABILITIES();

public:
    EnergyProducerModule();
    ~EnergyProducerModule();

    u32 ProducedEnergy() const;
};

} // namespace ECSEngine
