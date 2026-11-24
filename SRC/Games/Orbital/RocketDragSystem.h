#pragma once

#include "ECSCore\ModuleSystem.h"

namespace ECSEngine
{
class RocketDragSystem : public ModuleSystem
{
public:
    RocketDragSystem();

protected:
    void VirtualUpdate() override;
};
} // namespace ECSEngine
