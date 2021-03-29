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

public:
    OrientationModule()
        : Module()
        , FOrientation(1.0f, 0.0f, 0.0f, 0.0f)
    {
    }

    ~OrientationModule() { }

    const glm::quat& GetOrientation() const { return FOrientation; }
    const glm::vec3 GetOrientationAsYawPitchRoll() const;
    void SetOrientation(const glm::quat& parOrientation) { FOrientation = parOrientation; }

    const glm::vec3 Forward() const;
    const glm::vec3 Right() const;
    const glm::vec3 Up() const;

protected:
    void VirtualInit(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters);

private:
    glm::quat FOrientation;
};
} // namespace ECSEngine
