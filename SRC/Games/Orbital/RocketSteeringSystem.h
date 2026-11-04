#pragma once

#include "ECSCore\ModuleSystem.h"

namespace ECSEngine
{
class RocketSteeringSystem : public ModuleSystem
{
public:
    RocketSteeringSystem();

protected:
    void VirtualUpdate() override;
};
} // namespace ECSEngine