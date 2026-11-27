#pragma once

#include "ECSCore/ModuleSystem.h"

namespace ECSEngine
{
class DebrisLifetimeSystem : public ModuleSystem
{
public:
    DebrisLifetimeSystem();

protected:
    void VirtualUpdate() override;
};
} // namespace ECSEngine
