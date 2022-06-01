
#include "stdafx.h"

#include "BuildingGridOccupancyModule.h"

#include "Application/PropertyDrawer.h"
#include "ECSCore/EntityTemplateManager.h"
#include "ECSCore/ModuleUtils.h"
#include "ECSGameplay_Common/NavMeshPathfindingManager.h"
#include "Common/Polygon.h"
#include "ECSCore/ModuleParameters.h"

CEREAL_REGISTER_TYPE(ECSEngine::BuildingGridOccupancyModuleTemplate);
CEREAL_REGISTER_POLYMORPHIC_RELATION(ECSEngine::ModuleTemplate, ECSEngine::BuildingGridOccupancyModuleTemplate);

namespace ECSEngine
{
IMPLEMENT_MODULE_TEMPLATE(BuildingGridOccupancyModule, BuildingGridOccupancyModuleTemplate);

Module* BuildingGridOccupancyModuleTemplate::CreateInstance(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) const
{
    return NewModule<BuildingGridOccupancyModule>(this, parUnitId, parParameters);
}

void BuildingGridOccupancyModuleTemplate::VirtualDrawEditor()
{
    EDITOR_PROPERTY_SIMPLE("Number of cells used", FCellsOccupancy);
}

BuildingGridOccupancyModule::BuildingGridOccupancyModule()
    : Module()
{
}

BuildingGridOccupancyModule::~BuildingGridOccupancyModule()
{
}

void BuildingGridOccupancyModule::VirtualInit(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters)
{
    parent_type::VirtualInit(parUnitId, parParameters);

    FGridAccessor = parParameters.Get<ModuleParameters::GridAccessor>();
    AlwaysCheckedAssert(FGridAccessor.Valid());

    // Make a new obstacle
    const Polygon2D obstacle = FGridAccessor.CreatePolygon(Template<BuildingGridOccupancyModuleTemplate>()->CellsOccupancy());
    Pathfinding::AddObstacle(obstacle);
}

} // namespace ECSEngine
