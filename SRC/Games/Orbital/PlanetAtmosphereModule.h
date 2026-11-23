
#pragma once
#include "ECSCore/Module.h"
#include "ECSCore/ModuleTemplate.h"

namespace ECSEngine
{
class PlanetAtmosphereModuleTemplate : public ModuleTemplate
{
    DECLARE_MODULE_TEMPLATE(PlanetAtmosphereModule, PlanetAtmosphereModuleTemplate);

public:
    PlanetAtmosphereModuleTemplate()
        : ModuleTemplate()
    {
    }
    ~PlanetAtmosphereModuleTemplate() { }

    Module* CreateInstance(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) const override;

    SERIALIZE()
    {
        PROPERTYFIELD(AtmosphereHeight, 0.f);
        PROPERTYFIELD(SurfaceDensity, 0.f);
        PROPERTYFIELD(ScaleHeight, 0.f);
    }

    float AtmosphereHeight() const { return FAtmosphereHeight; }
    float SurfaceDensity() const { return FSurfaceDensity; }
    float ScaleHeight() const { return FScaleHeight; }

protected:
    void VirtualDrawEditor() override;

private:
    float FAtmosphereHeight = 0.f;
    float FSurfaceDensity = 0.f;
    float FScaleHeight = 0.f;
};

class PlanetAtmosphereModule : public Module
{
    DECLARE_MODULE(PlanetAtmosphereModule);

    DECLARE_SAVELOAD_ABILITIES();

public:
    PlanetAtmosphereModule();
    ~PlanetAtmosphereModule();

    float AtmosphereHeight() const;
    float SurfaceDensity() const;
    float ScaleHeight() const;
    float DensityAt(const float Altitude) const;
};

} // namespace ECSEngine
