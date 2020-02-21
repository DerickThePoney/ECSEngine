#pragma once
#include "ECSCore/Module.h"
namespace ECSEngine
{
class EntityId;
namespace ModuleParameters
{
class ParameterContainer;
}
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