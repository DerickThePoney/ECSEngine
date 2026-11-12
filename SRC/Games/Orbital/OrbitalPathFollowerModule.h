
#pragma once
#include "ECSCore/Module.h"
#include "ECSCore/ModuleTemplate.h"

namespace ECSEngine
{
class OrbitalPathFollowerModuleTemplate : public ModuleTemplate
{
    DECLARE_MODULE_TEMPLATE(OrbitalPathFollowerModule, OrbitalPathFollowerModuleTemplate);

public:
    OrbitalPathFollowerModuleTemplate()
        : ModuleTemplate()
    {
    }
    ~OrbitalPathFollowerModuleTemplate() { }

    Module* CreateInstance(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) const override;

    SERIALIZE()
    {
        PROPERTYFIELD(OrbitRadius, 0.f);
        PROPERTYFIELD(OrbitPhase, 0.f);
    }

    const float OrbitRadius() const { return FOrbitRadius; }
    const float OrbitPhase() const { return FOrbitPhase; }

protected:
    void VirtualDrawEditor() override;

private:
    float FOrbitRadius = 0.f;
    float FOrbitPhase = 0.f;
};

class OrbitalPathFollowerModule : public Module
{
    DECLARE_MODULE(OrbitalPathFollowerModule);

    DECLARE_SAVELOAD_ABILITIES();

public:
    OrbitalPathFollowerModule();
    ~OrbitalPathFollowerModule();

    const EntityId& ParentObject() const { return FParentObject; }

    const double CurrentTime() const { return FCurrentTime; }
    void SetCurrentTime(const double Value) { FCurrentTime = Value; }

    const double MeanMotion() const { return FMeanMotion; }
    void SetMeanMotion(const double Value) { FMeanMotion = Value; }

    bool Initialized() const { return FInitialized; }
    void SetInitialized() { FInitialized = true; }

    const float OrbitRadius() const;
    const float OrbitPhase() const;

protected:
    void VirtualInit(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) override;

private:
    EntityId FParentObject;
    double FCurrentTime = 0.f;
    double FMeanMotion = 0.f;

    bool FInitialized = false;
};

} // namespace ECSEngine
