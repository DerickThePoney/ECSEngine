
#pragma once
#include "ECSCore/Module.h"
#include "ECSCore/ModuleTemplate.h"

namespace ECSEngine
{
class LinkToWorkPlaceModuleTemplate : public ModuleTemplate
{
    DECLARE_MODULE_TEMPLATE(LinkToWorkPlaceModule, LinkToWorkPlaceModuleTemplate);

public:
    LinkToWorkPlaceModuleTemplate()
        : ModuleTemplate()
    {
    }
    ~LinkToWorkPlaceModuleTemplate() { }

    Module* CreateInstance(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) const override;

    SERIALIZE() { }

protected:
    void VirtualDrawEditor() override;
};

class LinkToWorkPlaceModule : public Module
{
    DECLARE_MODULE(LinkToWorkPlaceModule);

public:
    LinkToWorkPlaceModule();
    ~LinkToWorkPlaceModule();

    const EntityId& WorkPlaceId() const { return FWorkPlaceId; }
    void SetWorkPlaceId(const EntityId& parWorkPlaceId) { FWorkPlaceId = parWorkPlaceId; }

private:
    EntityId FWorkPlaceId;
};

} // namespace ECSEngine
