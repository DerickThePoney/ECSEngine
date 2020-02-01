#pragma once
#include "Module.h"

namespace ECSEngine
{
class PositionModule final : public Module
{
    DECLARE_MODULE(PositionModule);

public:
    PositionModule()
        : Module()
        , FPosition(0.0f)
    {
    }

    const glm::aligned_vec3& GetPosition3D() const { return FPosition; }
    void SetPosition3D(const glm::aligned_vec3& parPosition) { FPosition = parPosition; }

protected:
    void VirtualInit(const ModuleParameters::ParameterContainer& parParameters);

private:
    glm::aligned_vec3 FPosition;
};
} // namespace ECSEngine