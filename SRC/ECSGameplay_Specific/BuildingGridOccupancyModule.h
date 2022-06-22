
#pragma once
#include "CircularGridAccessor.h"
#include "ECSCore/Module.h"
#include "ECSCore/ModuleTemplate.h"

namespace ECSEngine
{
class BuildingGridOccupancyModuleTemplate : public ModuleTemplate
{
    DECLARE_MODULE_TEMPLATE(BuildingGridOccupancyModule, BuildingGridOccupancyModuleTemplate);

public:
    BuildingGridOccupancyModuleTemplate()
        : ModuleTemplate()
    {
    }
    ~BuildingGridOccupancyModuleTemplate() { }

    Module* CreateInstance(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) const override;

    SERIALIZE() { PROPERTYFIELD(CellsOccupancy, 1); }

    u32 CellsOccupancy() const { return FCellsOccupancy; }

protected:
    void VirtualDrawEditor() override;

private:
    u32 FCellsOccupancy = 1;
};

class BuildingGridOccupancyModule : public Module
{
    DECLARE_MODULE(BuildingGridOccupancyModule);

    DECLARE_SAVELOAD_ABILITIES();

public:
    BuildingGridOccupancyModule();
    ~BuildingGridOccupancyModule();

    const CircularGridAccessor& GridAccessor() const { FGridAccessor; }
    void SetGridAccessor(const CircularGridAccessor& parAccessor) { FGridAccessor = parAccessor; }

protected:
    void VirtualInit(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) override;
    void VirtualDeinit() override;

private:
    CircularGridAccessor FGridAccessor;
};

} // namespace ECSEngine
