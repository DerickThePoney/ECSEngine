
#include "stdafx.h"

#include "BuildingGridOccupancyModule.h"

#include "Application/PropertyDrawer.h"
#include "Common/SavingSystemImplementation.h"
#include "ECSCore/EntityTemplateManagerMethods.h"
#include "ECSCore/ModuleParameters.h"
#include "ECSCore/ModuleUtils.h"

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
}

void BuildingGridOccupancyModule::VirtualDeinit()
{
    parent_type::VirtualDeinit();

    const BuildingGridOccupancyModuleTemplate* t = Template<BuildingGridOccupancyModuleTemplate>();
    FGridAccessor.SetOccupied(false, t->CellsOccupancy());
}

IMPLEMENT_SAVELOAD_ABILITIES(BuildingGridOccupancyModule);
template<typename Chunk, bool isWriting>
void BuildingGridOccupancyModule::SaveLoad(Chunk& parChunk)
{
    parent_type::SaveLoad(parChunk);

    parChunk& FGridAccessor;
}

} // namespace ECSEngine
