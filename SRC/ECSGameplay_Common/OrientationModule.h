#pragma once
#include "ECSCore/Module.h"
namespace ECSEngine
{
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

private:
    glm::quat FOrientation;
};
} // namespace ECSEngine