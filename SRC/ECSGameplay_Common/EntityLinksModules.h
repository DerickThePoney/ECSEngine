#pragma once
#include "ECSCore/Module.h"
#include "ECSCore/ModuleTemplate.h"

namespace ECSEngine
{
namespace ModuleParameters
{
class ParameterContainer;
}

class EntityId;

class LinkToOwnerModuleTemplate : public ModuleTemplate
{
    DECLARE_MODULE_TEMPLATE(LinkToOwnerModule, LinkToOwnerModuleTemplate);

public:
    LinkToOwnerModuleTemplate()
        : ModuleTemplate()
    {
    }

    virtual Module* CreateInstance(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) const override;

protected:
    void VirtualDrawEditor() override;
};

class LinkToOwnerModule final : public Module
{
    DECLARE_MODULE(LinkToOwnerModule);
    DECLARE_SAVELOAD_ABILITIES();

public:
    LinkToOwnerModule()
        : Module()
    {
    }

    ~LinkToOwnerModule() { }

    void SetOwnerId(const EntityId& parOwnerId) { FOwnerId = parOwnerId; }
    const EntityId& OwnerId() const { return FOwnerId; }

protected:
    void VirtualInit(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) override;

private:
    EntityId FOwnerId;
};
} // namespace ECSEngine
