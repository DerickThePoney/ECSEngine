#pragma once
#include "ECSCore/ModuleSystem.h"

namespace ECSEngine
{
class OrientationSystem : public ModuleSystem
{
    using parent_type = ModuleSystem;

public:
    OrientationSystem();
    ~OrientationSystem();

protected:
    void VirtualUpdate();
};
} // namespace ECSEngine
