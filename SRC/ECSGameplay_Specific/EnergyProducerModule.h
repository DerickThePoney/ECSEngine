
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

    SERIALIZE() { }

protected:
    void VirtualDrawEditor() override;
};

class EnergyProducerModule : public Module
{
    DECLARE_MODULE(EnergyProducerModule);

public:
    EnergyProducerModule();
    ~EnergyProducerModule();
};

} // namespace ECSEngine
