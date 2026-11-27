#pragma once

#include "ECSCore/ModuleSystem.h"

namespace ECSEngine
{
class RocketStagingSystem : public ModuleSystem
{
public:
    RocketStagingSystem();

protected:
    void VirtualUpdate() override;
};
} // namespace ECSEngine
