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

class OrientationModuleTemplate : public ModuleTemplate
{
    DECLARE_MODULE_TEMPLATE(OrientationModule, OrientationModuleTemplate);

public:
    OrientationModuleTemplate()
        : ModuleTemplate()
    {
    }
    virtual ~OrientationModuleTemplate() { }

    virtual Module* CreateInstance(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) const override;

protected:
    virtual void VirtualDrawEditor() override;
};

class OrientationModule final : public Module
{
    DECLARE_MODULE(OrientationModule);

    DECLARE_SAVELOAD_ABILITIES();

public:
    OrientationModule()
        : Module()
        , FOrientation(1.0f, 0.0f, 0.0f, 0.0f)
    {
    }

    ~OrientationModule() { }

    const quat& GetOrientation() const { return FOrientation; }
    const vec3 GetOrientationAsYawPitchRoll() const;
    void SetOrientation(const quat& parOrientation) { FOrientation = parOrientation; }

    const vec3 Forward() const;
    const vec3 Right() const;
    const vec3 Up() const;

protected:
    void VirtualInit(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters);

private:
    quat FOrientation;
};
} // namespace ECSEngine
