#pragma once
#include "Module.h"
namespace ECSEngine
{
class OrientationModule final : public Module
{
public:
    OrientationModule()
        : Module()
        , FOrientation(1.0f, 0.0f, 0.0f, 0.0f)
    {
    }

private:
    glm::quat FOrientation;
};
} // namespace ECSEngine