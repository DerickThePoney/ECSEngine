#pragma once
#include "ECSCore/ModuleSystem.h"

namespace ECSEngine
{
class PhysicsFloatingOriginSystem : public ModuleSystem
{
public:
    PhysicsFloatingOriginSystem();

protected:
    void VirtualUpdate();
};
} // namespace ECSEngine