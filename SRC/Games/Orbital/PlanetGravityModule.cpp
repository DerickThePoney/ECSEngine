
#include "stdafx.h"

#include "PlanetGravityModule.h"

#include "Common/SavingSystemImplementation.h"
#include "ECSCore/EntityTemplateManagerMethods.h"
#include "ECSCore/ModuleUtils.h"

CEREAL_REGISTER_TYPE(ECSEngine::PlanetGravityModuleTemplate);
CEREAL_REGISTER_POLYMORPHIC_RELATION(ECSEngine::ModuleTemplate, ECSEngine::PlanetGravityModuleTemplate);

namespace ECSEngine
{
IMPLEMENT_MODULE_TEMPLATE(PlanetGravityModule, PlanetGravityModuleTemplate);

Module* PlanetGravityModuleTemplate::CreateInstance(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) const
{
    return NewModule<PlanetGravityModule>(this, parUnitId, parParameters);
}

void PlanetGravityModuleTemplate::VirtualDrawEditor()
{
    EDITOR_PROPERTY_SIMPLE("Planet radius", FPlanetRadius);
    EDITOR_PROPERTY_SIMPLE("Surface gravity", FSurfaceGravity);
}

IMPLEMENT_SAVELOAD_ABILITIES(PlanetGravityModule);
template<typename Chunk, bool isWriting>
void PlanetGravityModule::SaveLoad(Chunk& parChunk)
{
    parent_type::SaveLoad(parChunk);
}

PlanetGravityModule::PlanetGravityModule()
    : Module()
{
}

PlanetGravityModule::~PlanetGravityModule()
{
}

float PlanetGravityModule::PlanetRadius() const
{
    const PlanetGravityModuleTemplate* const Temp = Template<PlanetGravityModuleTemplate>();
    return Temp->PlanetRadius();
}

float PlanetGravityModule::SurfaceGravity() const
{
    const PlanetGravityModuleTemplate* const Temp = Template<PlanetGravityModuleTemplate>();
    return Temp->SurfaceGravity();
}

void PlanetGravityModule::VirtualInit(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters)
{
    parent_type::VirtualInit(parUnitId, parParameters);

    const PlanetGravityModuleTemplate* const Temp = Template<PlanetGravityModuleTemplate>();
    FPlanetGravityConstant = Temp->PlanetRadius() * Temp->PlanetRadius() * Temp->SurfaceGravity();
}

} // namespace ECSEngine
