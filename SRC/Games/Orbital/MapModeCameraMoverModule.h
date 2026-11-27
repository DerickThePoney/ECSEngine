#pragma once
#include "ECSCore/Module.h"
#include "ECSCore/ModuleTemplate.h"

namespace ECSEngine
{
class MapModeCameraMoverModuleTemplate : public ModuleTemplate
{
    DECLARE_MODULE_TEMPLATE(MapModeCameraMoverModule, MapModeCameraMoverModuleTemplate);

public:
    MapModeCameraMoverModuleTemplate()
        : ModuleTemplate()
    {
    }
    ~MapModeCameraMoverModuleTemplate() { }

    Module* CreateInstance(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) const override;

    SERIALIZE()
    {
        PROPERTYFIELD(CameraName, "");
        PROPERTYFIELD(InitialDistance, 2000.f);
        PROPERTYFIELD(InitialPitch, Pi() / 3.f);
        PROPERTYFIELD(InitialYaw, 0.f);
    }

    const std::string& CameraName() const { return FCameraName; }
    float InitialDistance() const { return FInitialDistance; }
    float InitialPitch() const { return FInitialPitch; }
    float InitialYaw() const { return FInitialYaw; }

protected:
    void VirtualDrawEditor() override;

private:
    std::string FCameraName;
    float FInitialDistance = 2000.f;
    float FInitialPitch = Pi() / 3.f;
    float FInitialYaw = 0.f;
};

class MapModeCameraMoverModule : public Module
{
    DECLARE_MODULE(MapModeCameraMoverModule);

    DECLARE_SAVELOAD_ABILITIES();

public:
    MapModeCameraMoverModule();
    ~MapModeCameraMoverModule();

    u32 CamId() const { return FCamId; }

    float Distance() const { return FDistance; }
    void SetDistance(const float Value) { FDistance = Value; }

    float Pitch() const { return FPitch; }
    void SetPitch(const float Value) { FPitch = Value; }

    float Yaw() const { return FYaw; }
    void SetYaw(const float Value) { FYaw = Value; }

    const vec3& Tangent() const { return FTangent; }
    void SetTangent(const vec3& Value) { FTangent = Value; }

    bool OrbitInitialised() const { return FOrbitInitialised; }
    void SetOrbitInitialised(bool Value) { FOrbitInitialised = Value; }

    bool DistanceFitted() const { return FDistanceFitted; }
    void SetDistanceFitted() { FDistanceFitted = true; }

    const EntityId& TargetedPlanet() const { return FTargetedPlanet; }
    void SetTargetedPlanet(const EntityId& Value) { FTargetedPlanet = Value; }

protected:
    void VirtualInit(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters);

private:
    float FDistance = 2000.f;
    float FPitch = Pi() / 3.f;
    float FYaw = 0.f;

    vec3 FTangent;

    EntityId FTargetedPlanet;

    u32 FCamId;

    bool FOrbitInitialised = false;
    bool FDistanceFitted = false;
};

} // namespace ECSEngine
