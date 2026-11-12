#pragma once

#include "ECSCore\ModuleSystem.h"

namespace ECSEngine
{
class RocketSASSystem : public ModuleSystem
{
public:
    RocketSASSystem();

protected:
    void VirtualUpdate() override;
};
} // namespace ECSEngine