#pragma once
#include "ECSCore/Module.h"
#include "ECSCore/ModuleTemplate.h"

namespace ECSEngine
{
class HousingPlaceModuleTemplate : public ModuleTemplate
{
    DECLARE_MODULE_TEMPLATE(HousingPlaceModule, HousingPlaceModuleTemplate);

public:
    HousingPlaceModuleTemplate()
        : ModuleTemplate()
    {
    }
    ~HousingPlaceModuleTemplate() { }

    Module* CreateInstance(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) const override;

    u32 MaxPlace() const { return FMaxPlace; }

    SERIALIZE() { PROPERTYFIELD(MaxPlace, 0); }

protected:
    void VirtualDrawEditor() override;

private:
    u32 FMaxPlace = 0;
};

class HousingPlaceModule final : public Module
{
    DECLARE_MODULE(HousingPlaceModule);

public:
    HousingPlaceModule();
    ~HousingPlaceModule();

    u32 MaxPlace() const { return Template<HousingPlaceModuleTemplate>()->MaxPlace(); }
    u32 RemainingFreeSpace() const;

    void AddNewResident(const EntityId& parUnitId);
    void RemoveResident(const EntityId& parUnitId);

    bool IsResidentInHere(const EntityId& parUnitId);

protected:
    void VirtualInit(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) override;

private:
    std::vector<EntityId> FAssociatedResidents;
};

} // namespace ECSEngine
