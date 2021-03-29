#pragma once
#include "ECSCore/Module.h"
#include "ECSCore/ModuleTemplate.h"

namespace ECSEngine
{
class LinkToHousingPlaceModuleTemplate : public ModuleTemplate
{
    DECLARE_MODULE_TEMPLATE(LinkToHousingPlaceModule, LinkToHousingPlaceModuleTemplate);

public:
    LinkToHousingPlaceModuleTemplate()
        : ModuleTemplate()
    {
    }
    ~LinkToHousingPlaceModuleTemplate() { }

    Module* CreateInstance(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) const override;

    SERIALIZE() { }

protected:
    void VirtualDrawEditor() override;
};

class LinkToHousingPlaceModule : public Module
{
    DECLARE_MODULE(LinkToHousingPlaceModule);

public:
    LinkToHousingPlaceModule();
    ~LinkToHousingPlaceModule();

    const EntityId& HouseId() const { return FHouseId; }
    void SetHouseId(const EntityId& parHouseId) { FHouseId = parHouseId; }

private:
    EntityId FHouseId;
};

} // namespace ECSEngine
