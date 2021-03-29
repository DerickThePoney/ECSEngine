#pragma once
#include "ECSCore/Module.h"
#include "ECSCore/ModuleTemplate.h"

namespace ECSEngine
{
class EntityId;
namespace ModuleParameters
{
class ParameterContainer;
}

class CameraMoverModuleTemplate : public ModuleTemplate
{
    DECLARE_MODULE_TEMPLATE(CameraMoverModule, CameraMoverModuleTemplate);

public:
    CameraMoverModuleTemplate()
        : ModuleTemplate()
        , FCameraName("")
        , FCameraMaxSpeed(0.f)
        , FCameraAcceleration(0.f)
        , FCameraRotationMaxSpeed(0.f)
        , FCameraRotationAcceleration(0.f)
    {
    }
    virtual ~CameraMoverModuleTemplate() { }

    virtual Module* CreateInstance(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) const override;

    const std::string& CameraName() const { return FCameraName; }
    const float CameraMaxSpeed() const { return FCameraMaxSpeed; }
    const float CameraAcceleration() const { return FCameraAcceleration; }
    const float CameraRotationMaxSpeed() const { return FCameraRotationMaxSpeed; }
    const float CameraRotationAcceleration() const { return FCameraRotationAcceleration; }

    SERIALIZE()
    {
        PROPERTYFIELD(CameraName, "");
        PROPERTYFIELD(CameraMaxSpeed, 0.f);
        PROPERTYFIELD(CameraAcceleration, 0.f);
        PROPERTYFIELD(CameraRotationMaxSpeed, 0.f);
        PROPERTYFIELD(CameraRotationAcceleration, 0.f);
    }

protected:
    virtual void VirtualDrawEditor() override;

private:
    std::string FCameraName;
    float FCameraMaxSpeed;
    float FCameraAcceleration;
    float FCameraRotationMaxSpeed;
    float FCameraRotationAcceleration;
};

class CameraMoverModule final : public Module
{
    DECLARE_MODULE(CameraMoverModule);

public:
    CameraMoverModule()
        : Module()
        , FCamId(-1)
        , FCurrentSpeed(0.f)
        , FCurrentRotationSpeed(0.f)
    {
    }

    u32 CamId() const { return FCamId; }

    const float CurrentSpeed() const { return FCurrentSpeed; }
    const float CurrentRotationSpeed() const { return FCurrentRotationSpeed; }

    void SetCurrentSpeed(const float parNewSpeed) { FCurrentSpeed = parNewSpeed; }
    void SetCurrentRotationSpeed(const float parNewSpeed) { FCurrentRotationSpeed = parNewSpeed; }

protected:
    virtual void VirtualInit(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters);

private:
    u32 FCamId;

    float FCurrentSpeed;
    float FCurrentRotationSpeed;
};
} // namespace ECSEngine
