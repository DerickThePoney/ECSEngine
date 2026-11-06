#pragma once
#include "ECSCore\ModuleSystem.h"

namespace ECSEngine
{
class PlanetGravitySystem : public ModuleSystem
{
protected:
    void VirtualInit() override;
    void VirtualUpdate() override;
};
} // namespace ECSEngine