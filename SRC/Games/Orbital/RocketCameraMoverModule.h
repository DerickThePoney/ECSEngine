
#pragma once
#include "ECSCore/Module.h"
#include "ECSCore/ModuleTemplate.h"

namespace ECSEngine
{
class RocketCameraMoverModuleTemplate : public ModuleTemplate
{
    DECLARE_MODULE_TEMPLATE(RocketCameraMoverModule, RocketCameraMoverModuleTemplate);

public:
    RocketCameraMoverModuleTemplate()
        : ModuleTemplate()
    {
    }
    ~RocketCameraMoverModuleTemplate() { }

    Module* CreateInstance(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) const override;

    SERIALIZE()
    {
        PROPERTYFIELD(CameraName, "");
        PROPERTYFIELD(InitialDistance, 30.f);
        PROPERTYFIELD(InitialPitch, 0.f);
        PROPERTYFIELD(InitialYaw, Pi());
    }

    const std::string& CameraName() const { return FCameraName; }
    float InitialDistance() const { return FInitialDistance; }
    float InitialPitch() const { return FInitialPitch; }
    float InitialYaw() const { return FInitialYaw; }

protected:
    void VirtualDrawEditor() override;

private:
    std::string FCameraName;
    float FInitialDistance = 30.f;
    float FInitialPitch = 0.f;
    float FInitialYaw = Pi();
};

class RocketCameraMoverModule : public Module
{
    DECLARE_MODULE(RocketCameraMoverModule);

    DECLARE_SAVELOAD_ABILITIES();

public:
    RocketCameraMoverModule();
    ~RocketCameraMoverModule();

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

    const EntityId& TargetedRocket() const { return FTargetedRocket; }

protected:
    void VirtualInit(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters);

private:
    float FDistance = 30.f;
    float FPitch = 0.f;
    float FYaw = Pi();

    vec3 FTangent;

    EntityId FTargetedRocket;

    u32 FCamId;

    bool FOrbitInitialised = false;
};

} // namespace ECSEngine
