
#pragma once
#include "ECSCore/Module.h"
#include "ECSCore/ModuleTemplate.h"

namespace ECSEngine
{
class PlanetGravityModuleTemplate : public ModuleTemplate
{
    DECLARE_MODULE_TEMPLATE(PlanetGravityModule, PlanetGravityModuleTemplate);

public:
    PlanetGravityModuleTemplate()
        : ModuleTemplate()
    {
    }
    ~PlanetGravityModuleTemplate() { }

    Module* CreateInstance(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) const override;

    SERIALIZE()
    {
        PROPERTYFIELD(PlanetRadius, 0.f);
        PROPERTYFIELD(SurfaceGravity, 0.f);
    }

    float PlanetRadius() const { return FPlanetRadius; }
    float SurfaceGravity() const { return FSurfaceGravity; }

protected:
    void VirtualDrawEditor() override;

private:
    float FPlanetRadius = 0.f;
    float FSurfaceGravity = 0.f;
};

class PlanetGravityModule : public Module
{
    DECLARE_MODULE(PlanetGravityModule);

    DECLARE_SAVELOAD_ABILITIES();

public:
    PlanetGravityModule();
    ~PlanetGravityModule();

    float PlanetGravityConstant() const { return FPlanetGravityConstant; }
    float PlanetRadius() const;
    float SurfaceGravity() const;

protected:
    void VirtualInit(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters);

private:
    float FPlanetGravityConstant = 0.f;
};

} // namespace ECSEngine
