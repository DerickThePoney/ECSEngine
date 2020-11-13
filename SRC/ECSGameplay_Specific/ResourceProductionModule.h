#pragma once
#include "ECSCore/Module.h"
#include "ECSCore/ModuleTemplate.h"

namespace ECSEngine
{
class EntityId;

namespace ModuleParameters
{
class ParameterContainer;
}

class ResourceProductionModuleTemplate : public ModuleTemplate
{
    DECLARE_MODULE_TEMPLATE(ResourceProductionModule, ResourceProductionModuleTemplate);

public:
    ResourceProductionModuleTemplate()
        : ModuleTemplate()
    {
    }

    virtual Module* CreateInstance(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) const override;

    template<class Archive>
    void serialize(Archive& ar)
    {
    }

protected:
    virtual void VirtualDrawEditor() override;
};

class ResourceProductionModule final : public Module
{
    DECLARE_MODULE(ResourceProductionModule);

public:
    ResourceProductionModule();
    ~ResourceProductionModule() { }

protected:
    void VirtualInit(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) override;
};
} // namespace ECSEngine
