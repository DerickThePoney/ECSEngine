#pragma once
#include "ECSCore/Module.h"
#include "ECSCore/ModuleTemplate.h"

namespace ECSEngine
{
class PeonSpawnModuleTemplate : public ModuleTemplate
{
    DECLARE_MODULE_TEMPLATE(PeonSpawnModule, PeonSpawnModuleTemplate);

public:
    PeonSpawnModuleTemplate()
        : ModuleTemplate()
    {
    }
    ~PeonSpawnModuleTemplate() { }

    Module* CreateInstance(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) const override;

    SERIALIZE() { }

protected:
    void VirtualDrawEditor() override;
};

class PeonSpawnModule : public Module
{
    DECLARE_MODULE(PeonSpawnModule)
public:
    PeonSpawnModule();
    ~PeonSpawnModule();

protected:
    void VirtualInit(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parContainer) override;
};
} // namespace ECSEngine