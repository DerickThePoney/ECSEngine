#pragma once
#include "Module.h"

namespace ECSEngine
{
class PositionModule : public Module
{
    DECLARE_MODULE(PositionModule);

public:
    PositionModule()
        : Module()
        , FPosition(0.0f)
    {
    }

    const glm::aligned_vec3& GetPosition3D() const { return FPosition; }

private:
    glm::aligned_vec3 FPosition;
};
} // namespace ECSEngine