#pragma once

#include "ECSCore/ModuleSystem.h"

namespace ECSEngine
{
class RocketMassSystem : public ModuleSystem
{
public:
    RocketMassSystem();

protected:
    void VirtualUpdate() override;
};
} // namespace ECSEngine