
#include "stdafx.h"

#include "PlanetAtmosphereModule.h"

#include "Common/SavingSystemImplementation.h"
#include "ECSCore/EntityTemplateManagerMethods.h"
#include "ECSCore/ModuleUtils.h"

CEREAL_REGISTER_TYPE(ECSEngine::PlanetAtmosphereModuleTemplate);
CEREAL_REGISTER_POLYMORPHIC_RELATION(ECSEngine::ModuleTemplate, ECSEngine::PlanetAtmosphereModuleTemplate);

namespace ECSEngine
{
IMPLEMENT_MODULE_TEMPLATE(PlanetAtmosphereModule, PlanetAtmosphereModuleTemplate);

Module* PlanetAtmosphereModuleTemplate::CreateInstance(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) const
{
    return NewModule<PlanetAtmosphereModule>(this, parUnitId, parParameters);
}

void PlanetAtmosphereModuleTemplate::VirtualDrawEditor()
{
    EDITOR_PROPERTY_SIMPLE("Atmosphere height", FAtmosphereHeight);
    EDITOR_PROPERTY_SIMPLE("Surface density", FSurfaceDensity);
    EDITOR_PROPERTY_SIMPLE("Scale height", FScaleHeight);
}

IMPLEMENT_SAVELOAD_ABILITIES(PlanetAtmosphereModule);
template<typename Chunk, bool isWriting>
void PlanetAtmosphereModule::SaveLoad(Chunk& parChunk)
{
    parent_type::SaveLoad(parChunk);
}

PlanetAtmosphereModule::PlanetAtmosphereModule()
    : Module()
{
}

PlanetAtmosphereModule::~PlanetAtmosphereModule()
{
}

float PlanetAtmosphereModule::AtmosphereHeight() const
{
    const PlanetAtmosphereModuleTemplate* const Temp = Template<PlanetAtmosphereModuleTemplate>();
    return Temp->AtmosphereHeight();
}

float PlanetAtmosphereModule::SurfaceDensity() const
{
    const PlanetAtmosphereModuleTemplate* const Temp = Template<PlanetAtmosphereModuleTemplate>();
    return Temp->SurfaceDensity();
}

float PlanetAtmosphereModule::ScaleHeight() const
{
    const PlanetAtmosphereModuleTemplate* const Temp = Template<PlanetAtmosphereModuleTemplate>();
    return Temp->ScaleHeight();
}

float PlanetAtmosphereModule::DensityAt(const float Altitude) const
{
    const float Height = AtmosphereHeight();
    const float Rho0 = SurfaceDensity();
    const float Scale = ScaleHeight();
    if (Height <= 0.f || Rho0 <= 0.f || Scale <= 0.f || Altitude >= Height)
        return 0.f;

    return Rho0 * expf(-Altitude / Scale);
}

} // namespace ECSEngine
