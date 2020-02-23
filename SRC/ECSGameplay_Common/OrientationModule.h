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
    DECLARE_MODULE_TEMPLATE(PositionModule);

public:
    OrientationModuleTemplate()
        : ModuleTemplate()
    {
    }

    virtual Module* CreateInstance(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) const override;
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

    ~OrientationModule() {}

    const glm::quat& GetOrientation() const { return FOrientation; }
    const glm::vec3 GetOrientationAsYawPitchRoll() const;
    void SetOrientation(const glm::quat& parOrientation) { FOrientation = parOrientation; }

protected:
    void VirtualInit(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters);

private:
    glm::quat FOrientation;
};
} // namespace ECSEngine