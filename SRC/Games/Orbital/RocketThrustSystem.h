#pragma once

#include "ECSCore\ModuleSystem.h"

namespace ECSEngine
{
class RocketThrustSystem : public ModuleSystem
{
public:
    RocketThrustSystem();

protected:
    void VirtualUpdate() override;
};
} // namespace ECSEngine