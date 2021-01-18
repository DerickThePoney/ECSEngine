
#pragma once
#include "CircularBuildingGrid.h"
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

protected:
    void VirtualDrawEditor() override;

private:
    u32 FCellsOccupancy = 1;
};

class BuildingGridOccupancyModule : public Module
{
    DECLARE_MODULE(BuildingGridOccupancyModule);

public:
    BuildingGridOccupancyModule();
    ~BuildingGridOccupancyModule();

    CircularGridAccessor& GridAccessor() { FGridAccessor; }
    void SetGridAccessor(const CircularGridAccessor& parAccessor) { FGridAccessor = parAccessor; }

protected:
    void VirtualInit(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) override;

private:
    CircularGridAccessor FGridAccessor;
};

} // namespace ECSEngine
