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

private:
    glm::aligned_vec3 FPosition;
};
} // namespace ECSEngine