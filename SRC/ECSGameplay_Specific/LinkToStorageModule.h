
#pragma once
#include "ECSCore/Module.h"
#include "ECSCore/ModuleTemplate.h"

namespace ECSEngine
{
class LinkToStorageModuleTemplate : public ModuleTemplate
{
    DECLARE_MODULE_TEMPLATE(LinkToStorageModule, LinkToStorageModuleTemplate);

public:
    LinkToStorageModuleTemplate()
        : ModuleTemplate()
    {
    }
    ~LinkToStorageModuleTemplate() { }

    Module* CreateInstance(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) const override;

    SERIALIZE() { }

protected:
    void VirtualDrawEditor() override;
};

class LinkToStorageModule : public Module
{
    DECLARE_MODULE(LinkToStorageModule);

public:
    LinkToStorageModule();
    ~LinkToStorageModule();

    const EntityId StorageId() const { return FStorageId; }
    void SetStorageId(const EntityId parId) { FStorageId = parId; }

protected:
    void VirtualInit(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) override;
    void VirtualDeinit() override;

private:
    EntityId FStorageId;
};

} // namespace ECSEngine
